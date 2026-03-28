
```
====================================================================================================
LINUX GPIO INTERRUPT HANDLING — COMPLETE SENIOR MENTAL MODEL
====================================================================================================

KEY QUESTION
----------------------------------------------------------------------------------------------------
How does a physical GPIO button press turn into driver code execution?

The full chain is:
    Electrical edge
        ↓
    GPIO controller detects edge
        ↓
    interrupt controller raises IRQ
        ↓
    Linux IRQ subsystem dispatches handler
        ↓
    driver ISR (top-half)
        ↓
    optional deferred processing
        ↓
    driver logic completes

Three distinct engineering problems appear:
    1) interrupt delivery
    2) signal debouncing
    3) latency management (top-half vs bottom-half)

====================================================================================================
LAYER 0 — PHYSICAL HARDWARE EVENT
====================================================================================================

Button connected to GPIO input.
Pressing the button changes the electrical state.
Example wiring:
        VCC
         │
        [R]
         │
GPIO20 ──┼─────── Button ───── GND

Normal state:
    GPIO = HIGH

Button press:
    GPIO = LOW

Electrical event:
    HIGH ───────┐
                └──── LOW

This falling edge is what the GPIO controller detects.

Important: the driver does NOT monitor the pin continuously.

The hardware detects the edge.

====================================================================================================
LAYER 1 — GPIO CONTROLLER DETECTS EDGE
====================================================================================================

Inside the SoC there is a GPIO controller.
The controller can detect:
    rising edge
    falling edge
    both edges
    level triggers

Driver configures the trigger:
    IRQF_TRIGGER_FALLING

Meaning:
    interrupt occurs when signal transitions
        HIGH → LOW

Hardware event:
    GPIO20 falling edge
        ↓
    GPIO controller signals interrupt controller

====================================================================================================
LAYER 2 — INTERRUPT CONTROLLER + IRQ NUMBER
====================================================================================================

Hardware interrupt lines are mapped to Linux IRQ numbers.
Example mapping:
    GPIO line 20
        ↓
    internal interrupt line
        ↓
    Linux IRQ number (example: 58)

Driver obtains this mapping:
    gpio_to_irq(button_gpio)
or
    gpiod_to_irq(desc)

Important concept:

    IRQ number ≠ GPIO number

Example:
    GPIO 532 → IRQ 58

IRQ numbers represent entries in the Linux IRQ subsystem.

====================================================================================================
LAYER 3 — DRIVER REGISTERS INTERRUPT HANDLER
====================================================================================================

Driver registers handler:
    request_irq(
        irq_number,
        button_isr,
        IRQF_TRIGGER_FALLING,
        "btn_irq_handler",
        NULL
    );

After registration the IRQ subsystem stores:

    IRQ 58
        handler = button_isr
        name    = btn_irq_handler

Visible in:
    /proc/interrupts

Example:
    58:  12  btn_irq_handler

Meaning:
    interrupt 58 fired 12 times
    handled by this driver

====================================================================================================
LAYER 4 — IRQ DISPATCH FLOW
====================================================================================================

Full runtime interrupt path:
    Button pressed
        ↓
    GPIO controller detects falling edge
        ↓
    interrupt controller raises IRQ
        ↓
    CPU interrupt vector invoked
        ↓
    Linux IRQ subsystem executes handler
        ↓
    button_isr()

This execution occurs in:
    HARD IRQ CONTEXT

Important properties:
    cannot sleep
    must run quickly
    interrupts temporarily disabled

====================================================================================================
TOP-HALF ISR RULES
====================================================================================================

ISR example:
    static irqreturn_t button_isr(int irq, void *dev_id)

Purpose:
    immediate response to interrupt

Allowed operations:
    read GPIO state
    update small variables
    schedule deferred work

Avoid:
    long processing
    sleeping
    blocking operations

Return value:
    IRQ_HANDLED

Meaning:
    this driver handled the interrupt.

====================================================================================================
PROBLEM — MECHANICAL BUTTON BOUNCE
====================================================================================================

Real switches do not produce clean transitions.
Ideal signal:
    HIGH ────────────────┐
                         └──── LOW

Actual signal:
    HIGH ─────┐ ┌─┐ ┌────┐
              └─┘ └─┘    └──── LOW

Each bounce may generate interrupts.
Example sequence:
    press button

interrupt events:
    IRQ1
    IRQ2
    IRQ3
    IRQ4

Driver ISR toggles LED each time.
Result:
    LED toggles multiple times.

Visible behavior:
    unpredictable LED state.

Problem source:
    mechanical contact bounce.

====================================================================================================
DEBOUNCE STRATEGY
====================================================================================================

Goal:
    convert many noisy interrupts into one logical press.

Approach used:
    timer-based validation.

Algorithm:
    interrupt occurs
        ↓
    record button state
        ↓
    schedule timer (20 ms)
        ↓
    if new interrupt occurs: restart timer
        ↓
    when timer expires: check stable state
        ↓
    accept as real button press

Key idea:
    wait for signal to stabilize.

====================================================================================================
DEBOUNCE IMPLEMENTATION FLOW
====================================================================================================

ISR:
    read button state
    save state
    restart timer

Code concept:
    mod_timer(&debounce_timer, jiffies + msecs_to_jiffies(20));

Important property:
    mod_timer() resets the timer if already running.

Bounce scenario:
    IRQ1 at t=0
        timer scheduled at t=20

    IRQ2 at t=5
        timer moved to t=25

    IRQ3 at t=10
        timer moved to t=30

After signal stabilizes:
    timer finally fires once.

====================================================================================================
TIMER CALLBACK VALIDATION
====================================================================================================

Timer callback:
    read button again
    compare with stored state

If same:
    valid press

Action:
    toggle LED

Event sequence now becomes:
    bounce IRQs
        ↓
    timer repeatedly restarted
        ↓
    signal stabilizes
        ↓
    one timer callback
        ↓
    LED toggled once

Bounce noise filtered.

====================================================================================================
TIME MODEL — JIFFIES
====================================================================================================

Timer scheduling:
    jiffies + msecs_to_jiffies(20)

Meaning:
    current kernel tick count
        +
    number of ticks representing 20 ms

Timer wheel resolution depends on:
    kernel HZ value.

Typical:
    HZ = 250

Meaning:
    4 ms tick resolution.

For debounce this is acceptable.

====================================================================================================
DEFERRED WORK (BOTTOM-HALF)
====================================================================================================

Second example introduces workqueue.
Purpose:
    move slow work out of ISR.

Workqueue initialization:
    INIT_WORK(&button_work, button_work_handler)

ISR now does:
    schedule_work(&button_work)

ISR returns immediately.
Later execution:
    worker thread executes handler.

====================================================================================================
TOP-HALF VS BOTTOM-HALF TIMELINE
====================================================================================================

Button press
    ↓
IRQ fired
    ↓
TOP-HALF (ISR)
    log event
    schedule_work()
    return quickly
    ↓
normal system continues
    ↓
WORKQUEUE THREAD
    runs button_work_handler()
    performs slow work toggles LED

Important difference:
    bottom-half runs in process context.

Allowed operations:
    sleeping
    blocking
    long operations

====================================================================================================
WHY SLEEPING IN ISR IS WRONG
====================================================================================================

Example incorrect code:
    msleep(1000) inside ISR

This breaks interrupt rules.
Reason:
    ISR runs in interrupt context.

Sleeping requires scheduler interaction.
Interrupt context cannot sleep safely.
Solution:
    move work to workqueue.

====================================================================================================
SAFE MODULE UNLOAD
====================================================================================================

Exit cleanup:
    free_irq()
    del_timer_sync()
    cancel_work_sync()

Purpose:
    ensure no asynchronous callbacks remain.

Why important:
    if callback runs after module unload kernel may jump to invalid memory.

Senior rule:
    every async mechanism must be stopped before unload.

Examples:
    IRQ
    timers
    workqueues
    kernel threads
    tasklets

====================================================================================================
COMPLETE SYSTEM VIEW
====================================================================================================

PHYSICAL WORLD
    button press
        ↓
    electrical edge

HARDWARE
    GPIO controller detects edge
        ↓
    interrupt controller signals CPU

KERNEL IRQ SUBSYSTEM
    IRQ number dispatched
        ↓
    ISR executed

DRIVER LOGIC
    ISR records event
        ↓
    debounce timer OR workqueue scheduled
        ↓
    later processing occurs
        ↓
    LED toggled

====================================================================================================
FINAL SENIOR TAKEAWAY
====================================================================================================

A GPIO interrupt driver solves three independent problems:

1) EVENT DELIVERY
   GPIO edge becomes Linux IRQ and invokes ISR.

2) SIGNAL VALIDATION
   debounce timer filters noisy switch transitions.

3) LATENCY MANAGEMENT
   ISR performs minimal work while slow processing is deferred to a bottom-half.

Complete mental model:
    electrical edge
        ↓
    GPIO controller
        ↓
    Linux IRQ number
        ↓
    ISR (top-half)
        ├── record event
        ├── arm debounce timer
        └── or schedule bottom-half
        ↓
    deferred context finishes processing
        ↓
    LED updated safely and correctly

====================================================================================================
```