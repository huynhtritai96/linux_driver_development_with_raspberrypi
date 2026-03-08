# GPIO Control

### Video :
[![Youtube Video](https://img.youtube.com/vi/wSmjAxeJr34/0.jpg)](https://www.youtube.com/watch?v=wSmjAxeJr34)


```
========================================================================================================
LINUX GPIO CONTROL IN A KERNEL MODULE — SENIOR MENTAL MODEL (ONE COMPLETE TEXT DIAGRAM)
========================================================================================================

GOAL OF THIS LESSON
────────────────────────────────────────────────────────────────────────────────────────────────────────
This is the first step from "foundation code" into "real hardware interaction".

The module is doing 4 practical things:

    1. identify which GPIO lines it wants to use
    2. configure one line as output  (LED)
    3. configure one line as input   (button)
    4. drive/read the pins during module load/unload

So the real engineering question is:

    "How does a kernel module take a GPIO line from the system, configure it safely, use it, and release it?"

========================================================================================================
PHYSICAL WORLD ↔ KERNEL WORLD ↔ DRIVER WORLD
========================================================================================================

PHYSICAL BOARD
──────────────
Raspberry Pi header pins / SoC GPIO lines

Example wiring in this lesson:
    GPIO 21  ─────► LED
    GPIO 20  ─────► Button

Meaning:
    one line is used as OUTPUT
    one line is used as INPUT

KERNEL REPRESENTATION
─────────────────────
GPIO controller hardware is exposed to Linux as GPIO chips.

Example observed from userspace:
    gpiodetect
        gpiochip0  -> pinctrl-bcm2711
        gpiochip1  -> raspberrypi-exp-gpio

So Linux does NOT think first in terms of:
    "header pin 40"

It thinks in terms of:
    "GPIO controller + line inside that controller"

DRIVER VIEW
───────────
The module needs a handle to the GPIO line, then it can:
    - request/claim it
    - set direction
    - set value if output
    - get value if input
    - release it when done

========================================================================================================
SYSTEM TOPOLOGY ON RASPBERRY PI
========================================================================================================

USERSPACE DISCOVERY
───────────────────
You ran:
    gpiodetect

This shows there are multiple GPIO chips.

Conceptually:

    gpiochip0  (main SoC GPIO controller)
        ├── line 0
        ├── line 1
        ├── ...
        └── line 57

    gpiochip1  (firmware/expander/internal-purpose GPIOs)
        ├── line 0
        ├── ...
        └── line 7

Important:
    not every line is a user header pin
    some are internal board/system signals

So the first senior lesson is:
    GPIO numbering at hardware level is controller-relative,
    not just "physical header numbering".

========================================================================================================
LEGACY GLOBAL GPIO NUMBER MODEL USED IN THIS MODULE
========================================================================================================

CODE
────
    #define LED_GPIO 21
    #define BUTTON_GPIO 20
    #define GPIO_OFFSET 512

    static int led_gpio    = LED_GPIO + GPIO_OFFSET;
    static int button_gpio = BUTTON_GPIO + GPIO_OFFSET;

MENTAL MODEL
────────────
This module uses the older "global GPIO number" style.

It says:
    gpiochip0 base = 512
    so:
        BCM GPIO 21 -> Linux global GPIO 533
        BCM GPIO 20 -> Linux global GPIO 532

Diagram:

    gpiochip0 base offset = 512
        line 20 -> global gpio 532
        line 21 -> global gpio 533

So when the driver calls:
    gpio_request(533, "led_gpio")

it is asking for:
    controller gpiochip0, line 21, through the legacy flattened numbering scheme

Important distinction:
    hardware line number:
        21
    legacy Linux global GPIO number:
        533

This offset-based scheme is exactly why the code defines GPIO_OFFSET.

========================================================================================================
TWO APIS SHOWN IN THIS LESSON
========================================================================================================

API 1 — LEGACY INTEGER GPIO API
───────────────────────────────
Headers:
    <linux/gpio.h>

Driver works with:
    integer GPIO numbers

Main functions:
    gpio_request()
    gpio_free()
    gpio_direction_output()
    gpio_direction_input()
    gpio_set_value()
    gpio_get_value()

Mental model:
    "I know the global gpio number; let me claim and manipulate it manually."

API 2 — DESCRIPTOR-BASED GPIO API
─────────────────────────────────
Headers:
    <linux/gpio/consumer.h>

Driver works with:
    struct gpio_desc *

Main functions:
    gpio_to_desc()
    gpiod_direction_output()
    gpiod_direction_input()
    gpiod_set_value()
    gpiod_get_value()

Mental model:
    "The kernel gives me a GPIO descriptor object; I operate through that handle."

Senior takeaway:
    the integer API is the old numbering-centric model
    the descriptor API is the newer object/handle-centric model

========================================================================================================
PART A — LEGACY REQUEST/FREE FLOW
========================================================================================================

MODULE LOAD
───────────
insmod request_free.ko
      │
      ▼
┌──────────────────────────────────────────────────────────────────────────────────────────────────────┐
│ my_init()                                                                                            │
│                                                                                                      │
│ 1. status = gpio_request(led_gpio, "led_gpio")                                                       │
│ 2. status = gpio_direction_output(led_gpio, 0)                                                       │
│ 3. status = gpio_request(button_gpio, "button_gpio")                                                 │
│ 4. status = gpio_direction_input(button_gpio)                                                        │
│ 5. gpio_set_value(led_gpio, 1)                                                                       │
│ 6. gpio_get_value(button_gpio)                                                                       │
│ 7. print init success                                                                                │
└──────────────────────────────────────────────────────────────────────────────────────────────────────┘

========================================================================================================
STEP 1 — gpio_request()
========================================================================================================

CODE
────
    status = gpio_request(led_gpio, "led_gpio");

MENTAL MODEL
────────────
This is the driver saying to the kernel:

    "I want exclusive ownership/use of this GPIO line."

What kernel is protecting:
    - prevents accidental double-use
    - records that some consumer/driver now owns this GPIO
    - gives visibility/debuggability through labels

Label purpose:
    "led_gpio"
is not the hardware name;
it is the consumer label, useful for debugging/ownership tracking

Diagram:

    before request:
        GPIO 533 -> free / unclaimed

    after successful request:
        GPIO 533 -> claimed by this module ("led_gpio")

Why request is needed in legacy flow:
    because just knowing the GPIO number is not enough;
    you should formally claim the resource before driving it

Output:
    driver now owns LED GPIO line

========================================================================================================
STEP 2 — gpio_direction_output()
========================================================================================================

CODE
────
    status = gpio_direction_output(led_gpio, 0);

MENTAL MODEL
────────────
This programs the GPIO controller so this line behaves as an OUTPUT.

The second parameter is the initial output value.

So:
    gpio_direction_output(led_gpio, 0)

means:
    configure line as output
    drive initial state LOW

Electrical meaning:
    output pin actively drives low level first

Why good practice:
    set a known safe initial state immediately when direction becomes output

Diagram:

    before:
        GPIO line mode unknown / prior configuration

    after:
        GPIO 21 -> OUTPUT, value 0

Output:
    LED line is under output control of driver, initially OFF/LOW

========================================================================================================
STEP 3 — request button GPIO
========================================================================================================

CODE
────
    status = gpio_request(button_gpio, "button_gpio");

Same conceptual model as LED:
    claim exclusive use of input pin before operating on it

Important failure handling:
    if button request fails after LED request already succeeded,
    LED must be freed before returning

That is why the code does:
    gpio_free(led_gpio);

Senior rule:
    resource acquisition must unwind already acquired resources on failure

========================================================================================================
STEP 4 — gpio_direction_input()
========================================================================================================

CODE
────
    status = gpio_direction_input(button_gpio);

MENTAL MODEL
────────────
This programs the GPIO controller so the line is sampled as INPUT.

Unlike output:
    no initial value parameter is needed,
    because the driver is not driving the line

Now the pin state will reflect external electrical condition:
    button not pressed -> maybe 0
    button pressed     -> maybe 1
depending on wiring and pull-up/pull-down design

Output:
    button line becomes readable input

========================================================================================================
STEP 5 — gpio_set_value()
========================================================================================================

CODE
────
    gpio_set_value(led_gpio, 1);

MENTAL MODEL
────────────
This drives the output line HIGH.

Since the line was already configured as output,
the module can now actively control the LED.

Diagram:

    driver
      │
      └── gpio_set_value(533, 1)
              │
              ▼
          GPIO controller
              │
              ▼
          electrical output HIGH
              │
              ▼
             LED ON

Output:
    LED turns on

========================================================================================================
STEP 6 — gpio_get_value()
========================================================================================================

CODE
────
    gpio_get_value(button_gpio)

MENTAL MODEL
────────────
This samples the current logical value seen on the input GPIO line.

Diagram:

    physical button state
         │
         ▼
    GPIO controller input latch
         │
         ▼
    gpio_get_value(button_gpio)
         │
         ▼
    driver gets 0 or 1

Output:
    module prints current button state in dmesg

This is a snapshot read, not interrupt-driven detection.
So it answers:
    "what is the input value right now during init?"

It does NOT yet mean:
    "notify me automatically when button changes"

That will come later with interrupts.

========================================================================================================
MODULE UNLOAD — LEGACY CLEANUP FLOW
========================================================================================================

rmmod request_free
      │
      ▼
┌──────────────────────────────────────────────────────────────────────────────────────────────────────┐
│ my_exit()                                                                                            │
│                                                                                                      │
│ 1. gpio_set_value(led_gpio, 0)                                                                       │
│ 2. gpio_free(led_gpio)                                                                               │
│ 3. gpio_free(button_gpio)                                                                            │
│ 4. print unload success                                                                              │
└──────────────────────────────────────────────────────────────────────────────────────────────────────┘

Why this order:
    - turn LED off before releasing control
    - then release claimed GPIO ownership

Meaning of gpio_free():
    this module no longer owns that GPIO resource

After free:
    line may later be requested by other code,
    or left unused depending on system configuration

Senior resource model:
    request -> configure -> use -> free

========================================================================================================
FAILURE UNWIND MODEL IN LEGACY VERSION
========================================================================================================

Acquisition order:
    1. request LED
    2. set LED direction
    3. request button
    4. set button direction

If a later step fails, earlier steps must be undone.

Examples:

If LED direction set fails:
    free LED
    return

If button request fails:
    free LED
    return

If button direction set fails:
    free LED
    free button
    return

That is correct kernel lifetime discipline:
    acquire forward
    release backward

========================================================================================================
PART B — DESCRIPTOR-BASED FLOW
========================================================================================================

MODULE LOAD
───────────
insmod descriptor_version.ko
      │
      ▼
┌──────────────────────────────────────────────────────────────────────────────────────────────────────┐
│ my_init()                                                                                            │
│                                                                                                      │
│ 1. led    = gpio_to_desc(led_gpio)                                                                   │
│ 2. button = gpio_to_desc(button_gpio)                                                                │
│ 3. gpiod_direction_output(led, 0)                                                                    │
│ 4. gpiod_direction_input(button)                                                                     │
│ 5. gpiod_set_value(led, 1)                                                                           │
│ 6. gpiod_get_value(button)                                                                           │
└──────────────────────────────────────────────────────────────────────────────────────────────────────┘

========================================================================================================
WHAT A GPIO DESCRIPTOR MEANS
========================================================================================================

TYPE
────
    struct gpio_desc *led;
    struct gpio_desc *button;

MENTAL MODEL
────────────
A gpio_desc is the kernel's internal object/handle representing a GPIO line.

Instead of:
    "GPIO number 533"

you now hold:
    "descriptor object for that line"

This is better because the kernel API can operate on a typed handle rather than a raw integer.

Diagram:

    legacy style:
        int gpio = 533

    descriptor style:
        struct gpio_desc *led ─────► kernel GPIO descriptor object for line 533

Why descriptors are better:
    - less tied to fragile global numbering
    - better fit for modern kernel frameworks
    - cleaner integration with device tree / ACPI / managed resources
    - more explicit object-oriented resource handling

========================================================================================================
gpio_to_desc()
========================================================================================================

CODE
────
    led = gpio_to_desc(led_gpio);
    button = gpio_to_desc(button_gpio);

MENTAL MODEL
────────────
This converts the old integer GPIO number into the kernel descriptor object.

So this lesson's descriptor example still starts from a global number,
but after conversion it uses the gpiod API.

Important subtle point:
    this is not the most modern full device-tree consumer flow yet,
    but it is a bridge toward descriptor-based thinking

Output:
    led/button become descriptor handles for later operations

========================================================================================================
gpiod_direction_output / gpiod_direction_input
========================================================================================================

Same conceptual behavior as legacy API, but now the argument is a descriptor.

Examples:

    gpiod_direction_output(led, 0)
        -> configure descriptor's line as output, initial low

    gpiod_direction_input(button)
        -> configure descriptor's line as input

Meaning:
    the operation is now bound to the GPIO object handle rather than raw integer id

========================================================================================================
gpiod_set_value / gpiod_get_value
========================================================================================================

Examples:

    gpiod_set_value(led, 1)
        -> drive output high

    gpiod_get_value(button)
        -> sample input

Functionally same lesson as before:
    one line controlled as output
    one line sampled as input

But the API style is more modern and kernel-recommended.

========================================================================================================
WHY THIS VERSION DOES NOT CALL gpio_free()
========================================================================================================

In this particular demo, the code uses:
    gpio_to_desc()

and then operates through descriptors,
but it does not use an explicit legacy gpio_request/gpio_free pair.

The tutorial explanation says:
    "we didn't request them explicitly, so we don't free them explicitly"

Senior interpretation:
    this is a simplified learning example contrasting the two API styles,
    not a full production-grade resource-managed consumer pattern yet.

In real modern drivers, the preferred pattern is usually:
    obtain descriptors through proper consumer APIs
    often device-managed (devm_*) if you have a struct device context

So the deep lesson is:
    this example is primarily showing the operational difference in API shape,
    not the final best-practice full platform-driver pattern yet.

========================================================================================================
COMPLETE SIGNAL FLOW FOR THIS LESSON
========================================================================================================

LED OUTPUT PATH
───────────────
module init
    │
    ├── acquire line handle
    ├── set as output, initial low
    ├── set value high
    ▼
GPIO controller drives line
    ▼
LED lights up

BUTTON INPUT PATH
─────────────────
external button electrical state
    ▼
GPIO controller samples line
    ▼
driver calls gpio_get_value / gpiod_get_value
    ▼
driver prints state in kernel log

========================================================================================================
WHAT THIS LESSON IS REALLY TEACHING
========================================================================================================

This is not yet an event-driven GPIO driver.
It is a synchronous configuration-and-snapshot example.

Meaning:
    OUTPUT side:
        configure line
        push a value

    INPUT side:
        configure line
        read current value once

It is not yet doing:
    - debounce logic
    - edge detection
    - interrupt registration
    - timer-based polling
    - user interface via char device/sysfs/debugfs

So architecturally this is the "GPIO basics" stage:
    claim/control/read lines directly inside module init/exit

========================================================================================================
OUTPUTS OF EACH STAGE
========================================================================================================

During load:
    - LED GPIO claimed or descriptor obtained
    - LED line configured as output
    - button line configured as input
    - LED turned ON
    - button state printed to dmesg

During unload:
    - LED turned OFF
    - GPIOs released in legacy version
    - unload message printed

Visible physical outputs:
    - LED state changes
    - button press changes logged sampled value

Kernel-visible outputs:
    - dmesg messages
    - resource ownership handled through GPIO subsystem

========================================================================================================
LEGACY API vs DESCRIPTOR API — SENIOR COMPARISON
========================================================================================================

LEGACY INTEGER GPIO API
───────────────────────
    gpio_request(gpio_num, label)
    gpio_direction_output(gpio_num, val)
    gpio_set_value(gpio_num, val)
    gpio_get_value(gpio_num)
    gpio_free(gpio_num)

Mental model:
    "I manually manage a numbered GPIO resource."

Pros:
    - simple for learning
    - easy to understand
    - useful for quick experiments

Cons:
    - tied to numbering assumptions
    - older interface
    - weaker abstraction
    - not ideal for portable modern drivers

DESCRIPTOR API
──────────────
    gpio_desc *desc
    gpiod_direction_output(desc, val)
    gpiod_set_value(desc, val)
    gpiod_get_value(desc)

Mental model:
    "Kernel gives me a GPIO object handle; I operate through that handle."

Pros:
    - newer and recommended model
    - better abstraction
    - integrates better with modern kernel frameworks
    - better direction for device-tree-based drivers

Cons in this tutorial stage:
    - still started from raw number via gpio_to_desc()
    - not yet showing full managed consumer acquisition path

========================================================================================================
FINAL SENIOR TAKEAWAY
========================================================================================================

This GPIO lesson is best understood as a hardware-resource ownership flow:

    identify the GPIO line
        ↓
    acquire a kernel handle to it
        ↓
    configure direction
        ↓
    drive output or sample input
        ↓
    release ownership when done

And it also introduces the API evolution:

    old model:
        raw GPIO number + manual request/free

    newer model:
        GPIO descriptor handle + gpiod operations

That is the core engineering mental model behind this Raspberry Pi GPIO control example.

========================================================================================================

```