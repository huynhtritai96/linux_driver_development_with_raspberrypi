

```
====================================================================================================
LINUX GPIO CONTROL IN A KERNEL MODULE — SENIOR MENTAL MODEL
====================================================================================================
KEY QUESTION
----------------------------------------------------------------------------------------------------
How does a Linux kernel module safely take control of a GPIO line, configure it, use it, and then release it without conflicting with the rest of the system?
This lesson answers four practical steps:
    1. identify the GPIO line
    2. acquire a kernel handle to it
    3. configure direction and state
    4. release it safely on exit

This is the first real hardware-resource management lesson.

====================================================================================================
LAYER 0 — PHYSICAL WORLD
====================================================================================================
On the Raspberry Pi board, you physically wire:
    GPIO 21  ---> LED
    GPIO 20  ---> Button

Meaning:
    GPIO 21 is an OUTPUT
    GPIO 20 is an INPUT

Physical behavior:
    OUTPUT line: the Pi actively drives voltage level
    INPUT line: the Pi samples external voltage level

COMMENT:
    At board level, this looks simple: "one pin for LED, one pin for button"

But Linux does not manage “pins” directly in the casual header sense.
It manages GPIO lines through GPIO controller hardware.

====================================================================================================
LAYER 1 — KERNEL VIEW OF GPIO
====================================================================================================
Linux sees GPIO through GPIO controller chips.
Example userspace discovery:
    gpiodetect

Possible result:
    gpiochip0  -> pinctrl-bcm2711
    gpiochip1  -> raspberrypi-exp-gpio

Meaning:
    Linux does not start with: "header pin 40"
    It starts with: "GPIO controller X, line Y"

Conceptual model:
    gpiochip0
        ├── line 0
        ├── line 1
        ├── ...
        └── line 57

    gpiochip1
        ├── line 0
        ├── ...
        └── line 7

Important:
    not every GPIO line is available on the external header
    some lines are internal to board/system functions

COMMENT:
This is the first big conceptual shift:
    board view: "I use GPIO 21"
    kernel view: "I use one line inside one GPIO controller"

That is why GPIO APIs are more abstract than just “pin number”.

====================================================================================================
LAYER 2 — DRIVER VIEW
====================================================================================================
A kernel module wants to do this:
    acquire line
        ↓
    configure direction
        ↓
    if output: drive value
    if input:  read value
        ↓
    release line on cleanup

This is resource ownership.
Senior mental model: a GPIO line is not just a number, it is a shared hardware resource

So good driver behavior means:
    claim it
    configure it
    use it
    release it

====================================================================================================
RASPBERRY PI NUMBERING IN THIS EXAMPLE
====================================================================================================
Code:
    #define LED_GPIO 21
    #define BUTTON_GPIO 20
    #define GPIO_OFFSET 512

    static int led_gpio    = LED_GPIO + GPIO_OFFSET;
    static int button_gpio = BUTTON_GPIO + GPIO_OFFSET;

Meaning: this example uses the old global GPIO numbering model

If gpiochip0 base is 512:
    BCM GPIO 20 -> Linux global GPIO 532
    BCM GPIO 21 -> Linux global GPIO 533

So:
    LED line    = 533
    Button line = 532

Important distinction:
    hardware-relative line:     21
    legacy Linux global number: 533

COMMENT: This often confuses beginners badly.

The code is NOT saying:
    GPIO 21 changed into another physical pin

It is saying:
    Linux legacy API flattened (chip base + line number) into one global integer namespace.

So 533 is not a new pin.
It is Linux’s old-style identifier for that GPIO line.

====================================================================================================
TWO API STYLES IN THIS LESSON
====================================================================================================
A) LEGACY INTEGER GPIO API
----------------------------------------------------------------------------------------------------
Header:
    <linux/gpio.h>

Works with:
    int gpio_number

Main functions:
    gpio_request()
    gpio_free()
    gpio_direction_output()
    gpio_direction_input()
    gpio_set_value()
    gpio_get_value()

Mental model: "I know the old global GPIO number; let me manage it directly."

B) DESCRIPTOR-BASED GPIO API
----------------------------------------------------------------------------------------------------
Header:
    <linux/gpio/consumer.h>

Works with:
    struct gpio_desc *

Main functions:
    gpio_to_desc()
    gpiod_direction_output()
    gpiod_direction_input()
    gpiod_set_value()
    gpiod_get_value()

Mental model:
    "The kernel gives me an object/handle representing the GPIO line."

Senior takeaway:
    legacy API = raw number-centric
    descriptor API = object/handle-centric

COMMENT:
The newer descriptor model is conceptually better because:
    it treats GPIO as a kernel object, not just an integer.

That fits better with:
    Device Tree
    ACPI
    device-managed resources
    modern driver model design

====================================================================================================
PART A — LEGACY REQUEST / CONFIGURE / USE / FREE
====================================================================================================

MODULE LOAD FLOW
----------------------------------------------------------------------------------------------------
insmod request_free.ko
    |
    v
my_init()
    |
    +--> gpio_request(led_gpio, "led_gpio")
    +--> gpio_direction_output(led_gpio, 0)
    +--> gpio_request(button_gpio, "button_gpio")
    +--> gpio_direction_input(button_gpio)
    +--> gpio_set_value(led_gpio, 1)
    +--> gpio_get_value(button_gpio)
    +--> print status

This is the complete legacy path.

====================================================================================================
STEP 1 — gpio_request()
====================================================================================================
Code:
    gpio_request(led_gpio, "led_gpio");

Meaning:
    "Kernel, I want exclusive ownership of this GPIO line."

Before request:
    GPIO 533 may be free or owned by something else

After successful request:
    GPIO 533 is claimed by this module

Why this matters:
    prevents accidental double use
    tracks ownership
    improves debugging

Label:
    "led_gpio"

This is not a hardware name.
It is a consumer label for visibility/debugging.

COMMENT: This is the hardware-resource equivalent of opening a file before using it.

You should not just assume a GPIO is yours.
You should claim it explicitly.

That is why `gpio_request()` exists.

====================================================================================================
STEP 2 — gpio_direction_output()
====================================================================================================
Code:
    gpio_direction_output(led_gpio, 0);

Meaning:
    configure the line as output and drive initial value LOW

Electrical effect:
    the SoC now actively drives the pin

Why pass initial value here:
    to place the line into a safe known state immediately

This is better than:
    configure output first
    set value later

Because there is no undefined window in between.

COMMENT:
This is a very senior hardware rule:
    when turning a pin into an output, choose the initial level deliberately

Why?
    Because some hardware reacts instantly.
    A temporary wrong level can glitch a device.

So “direction + initial value” is safer than two separate steps.

====================================================================================================
STEP 3 — REQUEST BUTTON INPUT
====================================================================================================
Code:
    gpio_request(button_gpio, "button_gpio");

Same meaning as LED request:
    claim ownership of this GPIO resource

Difference:
    this line will later be configured as input

Failure rule:
    if button request fails after LED request succeeded,
    LED must be freed before returning error

COMMENT:
Kernel resource handling rule:
    acquire resources forward
    unwind already-acquired resources backward on failure

This is one of the most important habits in driver programming.

====================================================================================================
STEP 4 — gpio_direction_input()
====================================================================================================
Code:
    gpio_direction_input(button_gpio);

Meaning:
    configure this line as input
    the controller will now sample external signal level

Unlike output:
    there is no initial output value because the driver is not driving the line

Observed value depends on:
    wiring
    pull-up / pull-down
    button state
    external electrical circuit

COMMENT:
This is another important conceptual difference:
    OUTPUT: kernel drives the world
    INPUT: kernel observes the world

That is the simplest clean mental model for GPIO direction.

====================================================================================================
STEP 5 — gpio_set_value()
====================================================================================================
Code:
    gpio_set_value(led_gpio, 1);

Meaning:
    drive output HIGH

Signal path:
    driver
        ↓
    GPIO subsystem
        ↓
    GPIO controller
        ↓
    electrical output on line
        ↓
    LED changes state

Result:
    LED turns on

COMMENT: Once direction is already OUTPUT, `gpio_set_value()` is no longer about ownership or mode.
It is just about changing the driven logic level.
====================================================================================================
STEP 6 — gpio_get_value()
====================================================================================================
Code:
    gpio_get_value(button_gpio);

Meaning:
    sample the current logical value seen at the input line

Signal path:
    external button state
        ↓
    GPIO controller input sample/latch
        ↓
    gpio_get_value()
        ↓
    driver gets 0 or 1

Important: this is a snapshot read
It answers: "What is the line level right now?"
It does NOT do:
    interrupts
    edge detection
    asynchronous notification

COMMENT: This lesson is synchronous GPIO usage. It is not yet event-driven GPIO.

So `gpio_get_value()` is just a direct sample, not a monitoring system.

====================================================================================================
LEGACY MODULE UNLOAD FLOW
====================================================================================================
rmmod request_free
    |
    v
my_exit()
    |
    +--> gpio_set_value(led_gpio, 0)
    +--> gpio_free(led_gpio)
    +--> gpio_free(button_gpio)
    +--> print unload message

Why this order:
    turn LED off first
    then release GPIO ownership

Meaning of gpio_free():
    this module no longer owns the line

COMMENT:
This is the inverse of module init:
    request
    configure
    use
    free

That symmetry is not accidental.
Good driver lifecycle should look like mirror-image cleanup.

====================================================================================================
FAILURE UNWIND MODEL
====================================================================================================
Resource acquisition order:
    1. request LED
    2. set LED direction
    3. request button
    4. set button direction

If something fails at step N:
    undo steps 1..N-1 in reverse order

Examples:
LED direction fails:
    free LED

Button request fails:
    free LED

Button direction fails:
    free button
    free LED

Senior rule: resource cleanup order should mirror acquisition order backwards
COMMENT: This is one of the strongest patterns in kernel programming. If you master this habit early, your code becomes much safer.

====================================================================================================
PART B — DESCRIPTOR-BASED GPIO FLOW
====================================================================================================
MODULE LOAD FLOW
----------------------------------------------------------------------------------------------------
insmod descriptor_version.ko
    |
    v
my_init()
    |
    +--> led    = gpio_to_desc(led_gpio)
    +--> button = gpio_to_desc(button_gpio)
    +--> gpiod_direction_output(led, 0)
    +--> gpiod_direction_input(button)
    +--> gpiod_set_value(led, 1)
    +--> gpiod_get_value(button)

Same physical behavior.
Different API style.

====================================================================================================
WHAT A GPIO DESCRIPTOR REALLY IS
====================================================================================================
Type:
    struct gpio_desc *led;
    struct gpio_desc *button;

Meaning:
    a gpio_desc is the kernel’s internal object/handle for one GPIO line

Old style:
    int gpio = 533

Newer style:
    struct gpio_desc *led  ---> kernel object for that GPIO line

Why this is better:
    less fragile than raw numbers
    fits modern kernel object design better
    works more naturally with DT/ACPI/resource-managed APIs

COMMENT:
The key mental upgrade is:
    raw number model: “I know line 533”
    descriptor model: “I hold a handle to the GPIO object”

That is a more kernel-native way to think.

====================================================================================================
gpio_to_desc()
====================================================================================================

Code:
    led = gpio_to_desc(led_gpio);
    button = gpio_to_desc(button_gpio);

Meaning:
    translate legacy integer identity into descriptor handle

Important nuance:
    this lesson still starts from legacy global numbers,
    then converts them into descriptors

So it is not yet the most modern full consumer-acquisition pattern.

COMMENT: This example teaches the API shape difference, not yet the full production-grade device-model acquisition pattern.

In real modern drivers, you often obtain descriptors from:
    Device Tree
    devm_gpiod_get()
    consumer lookup tables
not from `gpio_to_desc()` on hard-coded numbers.

====================================================================================================
gpiod_direction_output / gpiod_direction_input
====================================================================================================

Examples:
    gpiod_direction_output(led, 0)
    gpiod_direction_input(button)

Meaning:
    same conceptual behavior as legacy API
    but now bound to descriptor objects instead of raw integers

COMMENT:
    Same hardware effect.
    Better abstraction.

That is the real lesson here.

====================================================================================================
gpiod_set_value / gpiod_get_value
====================================================================================================

Examples:
    gpiod_set_value(led, 1)
    gpiod_get_value(button)

Meaning:
    same output driving and input sampling as before
    but using descriptor handle API

COMMENT:
    At this point the two APIs do the same job.
    The difference is how the GPIO line is represented in the driver.

====================================================================================================
WHY THIS DESCRIPTOR EXAMPLE DOES NOT CALL gpio_free()
====================================================================================================
Because this teaching example does not use:
    gpio_request()
    gpio_free()

It only converts raw number → descriptor
then operates on the descriptor.

Senior interpretation:
    this is a simplified educational contrast, not the final recommended production pattern

In real modern drivers, preferred pattern is usually:
    devm_gpiod_get(...)
    devm_gpiod_get_optional(...)
    etc.

That gives proper managed lifetime tied to `struct device`.

COMMENT: Very important: do not over-generalize this example into “descriptor APIs never need cleanup”.
That is not the real lesson.
The real lesson is: descriptor API is a better abstraction model

====================================================================================================
COMPLETE SIGNAL FLOW IN THIS LESSON
====================================================================================================

OUTPUT PATH
----------------------------------------------------------------------------------------------------
module init
    |
    +--> acquire/resolve line handle
    +--> configure as output
    +--> drive HIGH
    |
    v
GPIO controller drives electrical signal
    |
    v
LED turns on

INPUT PATH
----------------------------------------------------------------------------------------------------
external button state
    |
    v
GPIO controller samples line
    |
    v
driver calls get_value
    |
    v
driver logs state

====================================================================================================
WHAT THIS LESSON IS REALLY TEACHING
====================================================================================================

This is NOT yet:
    interrupt-driven GPIO
    event-driven GPIO
    debounced GPIO
    background-monitored GPIO

This IS:
    claim/control/read basic GPIO resources

Architecturally it teaches:
    GPIO as hardware resource ownership
        +
    direction configuration
        +
    synchronous value control / sampling

So this is the “GPIO fundamentals” stage.

====================================================================================================
OUTPUTS OF THE EXAMPLE
====================================================================================================

On load:
    - LED GPIO acquired / resolved
    - LED configured as output
    - button configured as input
    - LED turned ON
    - button state printed to dmesg

On unload:
    - LED turned OFF
    - legacy version frees GPIO ownership
    - unload message printed

Physical outputs:
    LED visible on/off behavior
    button press reflected in sampled value

Kernel-visible outputs:
    dmesg logs
    ownership changes in GPIO subsystem

====================================================================================================
LEGACY vs DESCRIPTOR API — SENIOR COMPARISON
====================================================================================================

LEGACY INTEGER API
----------------------------------------------------------------------------------------------------
    gpio_request(num, label)
    gpio_direction_output(num, val)
    gpio_set_value(num, val)
    gpio_get_value(num)
    gpio_free(num)

Mental model:
    manage GPIO through one flattened integer namespace

Pros:
    simple to learn
    explicit ownership calls
    easy for quick experiments

Cons:
    tied to numbering assumptions
    weaker abstraction
    older style
    not ideal for portable modern drivers

DESCRIPTOR API
----------------------------------------------------------------------------------------------------
    gpio_desc *desc
    gpiod_direction_output(desc, val)
    gpiod_set_value(desc, val)
    gpiod_get_value(desc)

Mental model:
    manage GPIO through kernel object handles

Pros:
    better abstraction
    aligns with modern kernel frameworks
    integrates naturally with DT/ACPI/device-managed resource models

Cons in this teaching example:
    still bootstrapped from legacy number via gpio_to_desc()
    not yet full modern acquisition pattern

====================================================================================================
FINAL SENIOR TAKEAWAY
====================================================================================================

This GPIO lesson is fundamentally about hardware-resource ownership:
    identify line
        ↓
    acquire kernel representation
        ↓
    configure direction safely
        ↓
    drive or sample logic level
        ↓
    release ownership when done

And it introduces the API evolution:
    old Linux style: raw global GPIO number
    newer Linux style: GPIO descriptor handle

So the deep mental model is:
    a GPIO is not "just a pin number";
    it is a kernel-managed hardware resource line owned through the GPIO subsystem.

====================================================================================================

```