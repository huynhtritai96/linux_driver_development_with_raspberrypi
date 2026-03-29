# GPIO Interrupt Handling

### Video :
[![Youtube Video](https://img.youtube.com/vi/hVp2e-uTYMs/0.jpg)](https://www.youtube.com/watch?v=hVp2e-uTYMs)

```
========================================================================================================
GPIO INTERRUPT HANDLING + DEBOUNCE + TOP-HALF/BOTTOM-HALF — SENIOR MENTAL MODEL (ONE COMPLETE DIAGRAM)
========================================================================================================

GOAL OF THIS LESSON
────────────────────────────────────────────────────────────────────────────────────────────────────────
This is the first real event-driven hardware example.

Previous GPIO lesson:
    driver actively set LED and actively sampled button once

This lesson:
    hardware button change generates an interrupt
    kernel calls driver ISR automatically
    driver reacts to asynchronous hardware events

So the real engineering question is:
    "How does a GPIO input become a Linux IRQ, how does the ISR run, why does button bounce break it, and how do timer debounce and bottom-half work solve different parts of the problem?"

========================================================================================================
PHYSICAL EVENT FLOW
========================================================================================================

Physical world:
    push button on GPIO 20
    LED on GPIO 21

Button press:
    electrical transition occurs on GPIO 20
        │
        ▼
GPIO controller detects configured edge
        │
        ▼
interrupt line raised inside SoC interrupt system
        │
        ▼
Linux IRQ subsystem dispatches registered handler
        │
        ▼
driver ISR (top-half) runs
        │
        ├── may do tiny immediate work
        ├── may arm debounce timer
        └── may schedule bottom-half work
        ▼
later processing happens
        │
        ├── debounce timer callback   OR
        └── workqueue bottom-half
        ▼
LED toggled / logic completed

Core mental shift:
    GPIO input is no longer just a pin value to poll 
    it becomes an asynchronous event source

========================================================================================================
PART 1 — GPIO LINE TO IRQ NUMBER
========================================================================================================

Two GPIO API styles are shown.

A) LEGACY INTEGER GPIO PATH
───────────────────────────
    button_gpio = BUTTON_GPIO + GPIO_OFFSET
    irq_number = gpio_to_irq(button_gpio)

Meaning:
    legacy global GPIO number
        ↓
    mapped by kernel GPIO subsystem
        ↓
    Linux IRQ number

Diagram:
    BCM GPIO 20
        ↓
    global GPIO 532 (because offset 512 + line 20)
        ↓
    gpio_to_irq(532)
        ↓
    IRQ number 58   (example on this system)

Important:
    IRQ 58 is NOT "GPIO 58"
    it is the kernel IRQ number assigned/mapped for that GPIO interrupt source

That is why after loading the module, /proc/interrupts shows a new entry with IRQ 58.

B) DESCRIPTOR GPIO PATH
───────────────────────
    button = gpio_to_desc(button_gpio)
    irq_number = gpiod_to_irq(button)

Meaning:
    descriptor handle for GPIO line
        ↓
    mapped directly to Linux IRQ number

Senior view:
    descriptor path is the modern object-based API
    legacy path is older raw-number-based API

========================================================================================================
PART 2 — REQUESTING THE IRQ
========================================================================================================

CODE SHAPE
──────────
    request_irq(irq_number,
                button_isr,
                IRQF_TRIGGER_FALLING,
                "btn_irq_handler",
                NULL);

Meaning of each field:

1. irq_number
    which IRQ line in Linux IRQ subsystem this driver wants

2. button_isr
    the top-half handler function to run immediately on interrupt

3. IRQF_TRIGGER_FALLING
    trigger policy: run when signal transitions from high to low

    In button hardware terms:
        pressing the button likely pulls the line low so falling edge means "button press event"

4. "btn_irq_handler"
    human-readable IRQ consumer name
    appears in /proc/interrupts

5. dev_id = NULL
    no per-device cookie used in this simple example

After successful request:
    Linux IRQ subsystem now owns this mapping:
        IRQ 58
          └── handler = button_isr
          └── name    = btn_irq_handler

So when hardware raises that IRQ:
    button_isr is invoked automatically by kernel interrupt path

========================================================================================================
PART 3 — TOP-HALF ISR (FAST INTERRUPT CONTEXT)
========================================================================================================

ISR SIGNATURE
─────────────
    static irqreturn_t button_isr(int irq, void *dev_id)

Why this signature:
    Linux IRQ subsystem expects handlers of irq_handler_t-compatible type

Inputs:
    irq     = which IRQ fired
    dev_id  = caller-supplied cookie from request_irq

Return:
    IRQ_HANDLED
        means: "yes, this interrupt belonged to me and I handled it"

Core rule of top-half:
    do as little as possible, as fast as possible

Why:
    interrupt context is timing-sensitive
    long operations here hurt system latency and responsiveness
    sleeping/blocking is not allowed in normal hard IRQ context

So top-half should mainly:
    - acknowledge/record minimal state
    - defer longer work
    - return quickly

========================================================================================================
PART 4 — SIMPLE IRQ VERSION (NO DEBOUNCE)
========================================================================================================

FIRST VERSION FLOW
──────────────────
button press
    ↓
IRQ fires
    ↓
button_isr()
    ├── print log
    ├── read current LED state
    ├── invert it
    └── set LED new value
    ↓
return IRQ_HANDLED

Diagram:
    GPIO 20 falling edge
         ↓
        IRQ 58
         ↓
       button_isr
         ↓
        LED = !LED

This works electrically, but a mechanical push button is not clean.

========================================================================================================
PART 5 — WHY BUTTON BOUNCE CAUSES MULTIPLE INTERRUPTS
========================================================================================================

REAL BUTTON PRESS IS NOT A PERFECT SINGLE EDGE
───────────────────────────────────────────────
When you press a mechanical switch, the contacts physically bounce.
Instead of one clean transition:
    ideal press:
        1 ────────────────┐
                          └──────── 0

real press may look like:
        1 ─────┐ ┌─┐ ┌────┐
               └─┘ └─┘    └──── 0

So one human press can generate multiple falling edges / transitions.

Interrupt subsystem sees:
    edge 1 -> IRQ
    edge 2 -> IRQ
    edge 3 -> IRQ
    ...

If ISR toggles LED on every interrupt: one button press may toggle multiple times

Example problem:
    expected: OFF -> ON
    actual due to bounce: OFF -> ON -> OFF -> ON
    final visible state may appear wrong

So the real issue is:
    interrupt hardware is correct
    physical button signal is noisy

========================================================================================================
PART 6 — DEBOUNCE TIMER MODEL
========================================================================================================

ADDED STATE
───────────
    #define DEBOUNCE_DELAY 20
    static int last_button_state;
    static struct timer_list debounce_timer;

Meaning:
last_button_state
    stores the state seen at interrupt time

debounce_timer
    delayed verification mechanism

DEBOUNCE STRATEGY
─────────────────
Do NOT trust the first instantaneous edge as final truth.
Instead:
    1. interrupt occurs
    2. record current button state
    3. start/restart timer for 20 ms
    4. after 20 ms, read button again
    5. if it still matches recorded state, accept as real press
    6. then toggle LED

This converts:
    noisy edge burst
into:
    one delayed validated state check

========================================================================================================
PART 7 — TOP-HALF WITH DEBOUNCE
========================================================================================================

ISR WITH TIMER
──────────────
button_isr():
    print interrupt occurred
    read current button state into last_button_state
    mod_timer(&debounce_timer, jiffies + msecs_to_jiffies(20))
    return IRQ_HANDLED

Meaning of mod_timer():
    "schedule this timer to expire 20 ms from now"
    and if already pending, move/restart it

This is important for bouncing:
    repeated interrupts during bounce keep re-arming the timer
    so validation happens only after the signal has had time to settle

Diagram of bounce handling:

    first bounce IRQ
        └── timer scheduled for +20ms

    second bounce IRQ 5ms later
        └── timer moved to now+20ms again

    third bounce IRQ 8ms later
        └── timer moved again

    signal finally stabilizes
        └── 20ms later callback runs once on settled state

This is the real debounce benefit:
    the timer acts like a "wait until noise stops" filter

========================================================================================================
PART 8 — TIMER CALLBACK (DEBOUNCED VALIDATION)
========================================================================================================

CALLBACK FLOW
─────────────
debounce_timer_callback():
    read current button state
    compare with last_button_state
    if equal:
        this is treated as a valid press
        toggle LED

Meaning:
    "The state observed now, after 20ms settling time, matches the state captured during IRQ"

So the LED toggling is intentionally moved OUT of the ISR.
New event flow:
    button edge burst
        ↓
    ISR records state + arms timer
        ↓
    timer expires after settling
        ↓
    callback validates stable state
        ↓
    LED toggled once

This solves switch bounce, not long-processing latency.

========================================================================================================
PART 9 — TIME REPRESENTATION: jiffies + msecs_to_jiffies()
========================================================================================================

TIMER SCHEDULING EXPRESSION
───────────────────────────
    jiffies + msecs_to_jiffies(DEBOUNCE_DELAY)

Meaning:
jiffies : current kernel tick count
msecs_to_jiffies(20) : convert 20 milliseconds into kernel ticks

So combined meaning: "expire 20 ms from current kernel time"

This is a low-resolution kernel-timer scheduling model, good enough for debounce-scale delays like 20 ms.

========================================================================================================
PART 10 — CLEANUP OF IRQ + TIMER
========================================================================================================

MODULE EXIT
───────────
    free_irq(irq_number, NULL);
    del_timer_sync(&debounce_timer);

Meaning:
free_irq : unregister this handler from Linux IRQ subsystem after this, button IRQ no longer invokes this module's ISR
del_timer_sync : stop timer safely and wait if callback is currently running

Why order matters conceptually: on unload, no asynchronous activity should still point into module code

Senior lifetime rule:
    any asynchronous kernel mechanism you registered (IRQ, timer, workqueue, thread, tasklet, etc.) must be shut down safely before module memory/code disappears

========================================================================================================
PART 11 — DESCRIPTOR API VERSION OF IRQ FLOW
========================================================================================================

WHEN USE_DESCRIPTOR_API IS ENABLED
──────────────────────────────────
    led    = gpio_to_desc(led_gpio);
    button = gpio_to_desc(button_gpio);

    gpiod_direction_output(led, 0);
    gpiod_direction_input(button);

    irq_number = gpiod_to_irq(button);

ISR reads input by:
    gpiod_get_value(button)

Timer callback toggles output by:
    gpiod_set_value(led, !gpiod_get_value(led))

Same architecture as legacy version, but object-handle based.

Mental mapping:
    legacy style:
        raw int gpio number
            ↓
        gpio_to_irq / gpio_get_value / gpio_set_value

    descriptor style:
        struct gpio_desc * handle
            ↓
        gpiod_to_irq / gpiod_get_value / gpiod_set_value

Senior takeaway:
    the interrupt architecture is the same
    only the GPIO handle API changes

========================================================================================================
PART 12 — TOP-HALF vs BOTTOM-HALF (SEPARATE EXAMPLE)
========================================================================================================

SECOND MODULE INTRODUCES WORKQUEUE BOTTOM-HALF
───────────────────────────────────────────────
Added: static struct work_struct button_work;
Initialized by: INIT_WORK(&button_work, button_work_handler);
ISR becomes: schedule_work(&button_work);

Bottom-half handler:
    button_work_handler()
        ├── print message
        ├── msleep(1000)
        └── toggle LED

This is a different lesson from debounce.

DEBOUNCE TIMER lesson answers: "How do I filter noisy repeated mechanical edges?"
BOTTOM-HALF lesson answers:"How do I move slow work out of interrupt context?"

These are related but not the same problem.

========================================================================================================
PART 13 — TOP-HALF / BOTTOM-HALF EVENT FLOW
========================================================================================================

Button press
    ↓
IRQ arrives
    ↓
TOP-HALF: button_isr()
    ├── log interrupt
    ├── schedule_work(&button_work)
    └── return immediately
    ↓
CPU can resume normal interrupt responsiveness
    ↓
Later, in process/workqueue context
    ↓
BOTTOM-HALF: button_work_handler()
    ├── msleep(1000)     <-- deliberately slow, allowed here
    └── toggle LED

Why this split exists:
    hard IRQ context must stay short and non-blocking
    slow processing belongs in deferred context

So:
top-half
    = urgent reaction
    = minimal work
    = cannot sleep normally

bottom-half
    = deferred processing
    = can do slower work
    = safer for lengthy operations

========================================================================================================
PART 14 — WHY msleep(1000) IN ISR WOULD BE WRONG
========================================================================================================

If you tried this inside ISR:
    msleep(1000);

That would be invalid design because ISR runs in interrupt context.

Interrupt context rules:
    - should finish quickly
    - must not do long blocking operations
    - sleeping there is generally illegal/unsafe

So the workqueue bottom-half example is teaching the correct separation:
    interrupt occurs now
        ↓
    ISR schedules later work
        ↓
    later work does the slow part

========================================================================================================
PART 15 — CLEANUP OF BOTTOM-HALF WORK
========================================================================================================

SECOND MODULE EXIT
──────────────────
    free_irq(irq_number, NULL);
    cancel_work_sync(&button_work);

Meaning of cancel_work_sync(): if work is pending or running, cancel/wait safely before module unload

This is the workqueue equivalent of safe async teardown.

Again, the senior rule is the same: no deferred execution may survive module removal

========================================================================================================
PART 16 — COMPLETE ARCHITECTURE MAP
========================================================================================================

PHYSICAL BUTTON/LED LEVEL
─────────────────────────
button press changes electrical signal on GPIO 20
LED reflects driven state on GPIO 21

GPIO SUBSYSTEM LEVEL
────────────────────
GPIO line is configured as input/output
input line can be mapped to IRQ
output line can be driven by driver

IRQ SUBSYSTEM LEVEL
───────────────────
mapped GPIO event becomes IRQ number
request_irq registers handler
IRQ dispatch invokes ISR

DEFERRED PROCESSING LEVEL
─────────────────────────
Option A: timer debounce callback validates stable button state later
Option B: workqueue bottom-half performs slow work later

DRIVER ACTION LEVEL
───────────────────
toggle LED
log messages
clean up resources on unload

========================================================================================================
PART 17 — OUTPUTS OF EACH STAGE
========================================================================================================

On module load:
    - LED/button GPIO configured
    - GPIO input mapped to IRQ number
    - request_irq succeeds
    - /proc/interrupts shows new entry
    - dmesg prints irq_number

On raw button press without debounce:
    - ISR may run multiple times due to switch bounce
    - LED may toggle multiple times unexpectedly

On button press with debounce timer:
    - ISR may still fire multiple times
    - timer keeps getting re-armed
    - only stable state after 20ms is accepted
    - LED toggles once per valid press

On button press with top-half/bottom-half example:
    - ISR log appears immediately
    - LED changes after 1 second
    - proves deferred work path is working

On module unload:
    - IRQ removed from /proc/interrupts
    - timer/work cancelled safely
    - LED turned off
    - GPIO resources freed in legacy version

========================================================================================================
PART 18 — ONE-LINE SENIOR DISTINCTIONS
========================================================================================================

gpio_to_irq / gpiod_to_irq = map GPIO event source to Linux IRQ identity
request_irq = register top-half handler with IRQ subsystem
ISR / top-half = immediate, minimal, fast interrupt reaction
debounce timer = filter noisy repeated edges from mechanical button bounce
workqueue bottom-half = move slow processing out of interrupt context
free_irq / del_timer_sync / cancel_work_sync = shut down asynchronous callbacks safely before unload

========================================================================================================
FINAL SENIOR TAKEAWAY
========================================================================================================

This lesson is best understood as three stacked concepts:
    1. GPIO INPUT AS EVENT SOURCE
       button GPIO is mapped into a Linux IRQ and handled asynchronously

    2. DEBOUNCE AS SIGNAL VALIDATION
       interrupt may fire many times, but timer-based delayed verification turns noisy edges into one
       logical press

    3. TOP-HALF / BOTTOM-HALF AS LATENCY CONTROL
       ISR reacts immediately and minimally; slower work is deferred to a safe later context

So the full mental model is:
    button electrical edge
        ↓
    GPIO controller
        ↓
    Linux IRQ number
        ↓
    top-half ISR
        ├── record state / arm timer
        └── or schedule bottom-half
        ↓
    deferred processing validates or completes work
        ↓
    LED state changes correctly and safely

========================================================================================================

```

