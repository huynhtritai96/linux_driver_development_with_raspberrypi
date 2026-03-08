```
========================================================================================================
LINUX I2C SUBSYSTEM — SENIOR MENTAL MODEL (ONE COMPLETE TEXT DIAGRAM)
========================================================================================================

GOAL OF THIS LESSON
────────────────────────────────────────────────────────────────────────────────────────────────────────
This lesson is not yet "how to write the BMP180 driver".
It is the architecture lesson that explains:

    1. what I2C is electrically
    2. how Linux models an I2C bus
    3. how user space and kernel space both see the same subsystem differently
    4. how adapters, clients, and drivers fit together
    5. how Device Tree and sysfs participate in device creation

So the real engineering question is:

    "When an I2C sensor is connected to a Linux board, how does that physical device become a Linux
     kernel object, and how does a driver finally get its probe() called?"

========================================================================================================
PART 1 — PHYSICAL I2C BUS MODEL
========================================================================================================

HARDWARE LEVEL
──────────────
I2C bus uses two wires:

    SDA  -> serial data
    SCL  -> serial clock

Both lines are open-drain / open-collector style:
    devices pull the line low
    pull-up resistors bring the line high when nobody drives low

Basic topology:

        VCC
         │
      [pull-up]
         │
SDA  ────┼─────────────────────────────────────────────────────────────── shared bus
         │         │               │                │
         │         │               │                │
      master     slave A         slave B          slave C

        VCC
         │
      [pull-up]
         │
SCL  ────┼─────────────────────────────────────────────────────────────── shared bus
         │         │               │                │
         │         │               │                │
      master     slave A         slave B          slave C

Important properties:
    - one shared bus for multiple devices
    - each slave has an address
    - master generates clock
    - transactions are address-based
    - after each 8 data bits, receiver returns ACK/NACK

Linux-driver developer viewpoint:
    you usually do NOT bit-bang SDA/SCL yourself
    you ask the Linux I2C subsystem to perform transfers for you

========================================================================================================
PART 2 — LINUX DOES NOT MODEL I2C AS "JUST PINS"
========================================================================================================

At board level, you may think:
    "BMP180 is wired to SDA/SCL"

At Linux kernel level, the model is higher-level:

    I2C controller hardware
        ↓
    Linux I2C adapter
        ↓
    I2C bus number
        ↓
    I2C client device at address X
        ↓
    I2C client driver that matches device identity

So Linux abstracts away raw wire signaling and instead provides a bus framework.

Senior takeaway:
    as a client-driver author, you usually write device logic, not bus signaling logic

========================================================================================================
PART 3 — THE FOUR CORE OBJECTS IN THE LINUX I2C MODEL
========================================================================================================

The lesson divides Linux I2C into bus side and device side.

BUS SIDE
────────
    struct i2c_adapter
        represents the I2C controller instance / bus master exposed by the system

    struct i2c_algorithm
        describes how transfers are actually executed by that adapter

DEVICE SIDE
───────────
    struct i2c_client
        represents one slave device instance on one bus at one address

    struct i2c_driver
        represents driver logic for a class/type of slave device

This is the single most important architecture diagram:

    PHYSICAL CONTROLLER HARDWARE
            │
            ▼
    i2c_adapter  -------------------+
            │                       |
            │ uses                  | registered with
            ▼                       |
    i2c_algorithm                   |
                                    ▼
                                 I2C CORE
                                    ▲
                                    |
                 +------------------+------------------+
                 |                                     |
                 | matches                             | registers
                 ▼                                     ▼
            i2c_client ---------------------------> i2c_driver

Meaning:
    adapter = "which bus master / which I2C bus"
    client  = "which slave device at which address"
    driver  = "who knows how to operate that slave"
    core    = "the matchmaker and common infrastructure"

========================================================================================================
PART 4 — WHAT EACH STRUCT REALLY MEANS
========================================================================================================

1) i2c_adapter
──────────────
Represents:
    one I2C bus controller instance from Linux's point of view

Not just "hardware exists", but:
    "the kernel has registered this bus master and assigned it an adapter number"

Typical user-visible result:
    /dev/i2c-1
    /sys/class/i2c-dev/i2c-1

So adapter is what becomes the bus visible to both kernel and user space.

Mental model:
    adapter = Linux object for "bus #N"

2) i2c_algorithm
────────────────
Represents:
    transfer methods supported by the adapter

It defines low-level transfer behavior, such as:
    how a transaction is performed
    which operations are supported

As a client-driver author, you usually do not implement this.
This is usually owned by the controller/bus driver.

Mental model:
    algorithm = "how this controller knows how to speak I2C"

3) i2c_client
─────────────
Represents:
    one actual slave device instance

Identity of a client is basically:

    (which adapter/bus, which slave address)

Example:
    bus 1, address 0x77
        -> one BMP180 instance

Example sysfs name:
    1-0077
means:
    bus 1
    address 0x77

Mental model:
    client = "this concrete chip on this concrete bus address"

4) i2c_driver
─────────────
Represents:
    the device-specific logic

It contains:
    - device ID tables / OF match tables
    - probe()
    - remove()
    - read/write logic built using I2C subsystem APIs

Mental model:
    driver = "software brain for this type of slave device"

========================================================================================================
PART 5 — THE I2C CORE AS THE MATCHMAKER
========================================================================================================

The I2C core sits between bus drivers and client drivers.

It handles:
    - registration/removal of adapters
    - registration/removal of drivers
    - matching clients to drivers
    - generic transfer APIs

Think of it like this:

    controller driver says:
        "I have an I2C bus"

    board description / sysfs / device creation says:
        "There is a client device at address 0x77 on that bus"

    client driver says:
        "I support bmp180"

    I2C core checks:
        "Do these belong together?"

If yes:
    core calls driver's probe()

This is the real probe path:

    adapter appears
        ↓
    client device exists or is created
        ↓
    driver is registered
        ↓
    I2C core matches them
        ↓
    probe(client) runs

Senior takeaway:
    your driver never probes "randomly"
    the I2C core is what decides when a concrete client and a concrete driver match

========================================================================================================
PART 6 — THREE-LAYER KERNEL VIEW
========================================================================================================

The lesson correctly splits kernel I2C architecture into three big parts:

    1. I2C core
    2. I2C bus driver
    3. I2C client driver

Detailed mental map:

    +-----------------------------------------------------------------------------------+
    | I2C CORE                                                                          |
    |-----------------------------------------------------------------------------------|
    | register adapters | register drivers | create clients | match client<->driver     |
    | common APIs such as i2c_transfer()                                                |
    +-----------------------------------------------------------------------------------+
             ▲                                                       ▲
             │                                                       │
             │                                                       │
    +-----------------------------+                        +-----------------------------+
    | I2C BUS DRIVER              |                        | I2C CLIENT DRIVER           |
    |-----------------------------|                        |-----------------------------|
    | struct i2c_adapter          |                        | struct i2c_driver           |
    | struct i2c_algorithm        |                        | struct i2c_client           |
    | controller hardware logic   |                        | sensor/eeprom/etc logic     |
    +-----------------------------+                        +-----------------------------+

Senior reading:
    bus driver owns "how bytes move on this controller"
    client driver owns "what these register reads/writes mean for this chip"

========================================================================================================
PART 7 — WHERE THE CODE LIVES IN THE KERNEL TREE
========================================================================================================

The lesson points to drivers/i2c.

Senior mental structure:

    drivers/i2c/
        core framework files
        generic user-space bridge
        bus/controller drivers
        legacy/special device drivers
        transfer algorithms

Important directories mentally:

    drivers/i2c/i2c-core*
        core framework

    drivers/i2c/i2c-dev.c
        bridge that exposes adapters as /dev/i2c-*

    drivers/i2c/busses/
        adapter/controller drivers

So if debugging:
    "my bus is missing"
        -> look toward controller/adapter driver and DT enablement

    "my device node exists but probe not called"
        -> look toward client description / match table / driver registration

========================================================================================================
PART 8 — TWO USAGE MODES IN LINUX
========================================================================================================

MODE A — USER SPACE THROUGH i2c-dev
───────────────────────────────────
User space talks to:
    /dev/i2c-1
    /dev/i2c-0
    etc.

Typical tooling:
    i2cdetect
    i2cget
    i2cset
    custom user programs

Meaning:
    user process opens the adapter device node
    then directly sends I2C transactions via ioctl/read/write interfaces

Use cases:
    - quick testing
    - board bring-up
    - debugging
    - prototyping

Mental model:
    i2c-dev = generic bridge from user space into I2C core

MODE B — DEDICATED KERNEL CLIENT DRIVER
───────────────────────────────────────
Kernel module registers an i2c_driver.
The I2C core binds that driver to matching clients.

Use cases:
    - production
    - integrated kernel subsystem support
    - better lifecycle management
    - tighter performance and error handling
    - proper sensor/input/hwmon integration

Mental model:
    dedicated client driver = correct long-term kernel-native solution

Senior takeaway:
    i2c-dev is for testing and generic access
    i2c_client driver is for real product-quality device support

========================================================================================================
PART 9 — USER SPACE VIEW OF THE SAME SUBSYSTEM
========================================================================================================

After enabling I2C in Raspberry Pi config, adapters appear as:

    /dev/i2c-1
    /dev/i2c-0
    ...

Each /dev/i2c-* corresponds to one i2c_adapter registered in kernel.

So the mapping is:

    kernel i2c_adapter #1
        ↔
    /dev/i2c-1
        ↔
    /sys/class/i2c-dev/i2c-1

This is the key bridge:

    USER SPACE                                KERNEL SPACE
    ──────────                                ────────────
    /dev/i2c-1      <-------------------->    i2c-dev driver
                                                    │
                                                    ▼
                                                 I2C core
                                                    │
                                                    ▼
                                             i2c_adapter #1

Important:
    /dev/i2c-1 is not the sensor device
    it is the BUS access endpoint

That distinction is critical.

========================================================================================================
PART 10 — WHAT i2cdetect -l REALLY SHOWS
========================================================================================================

Command:
    i2cdetect -l

Shows:
    all adapters registered with the kernel

So it answers:
    "Which I2C buses currently exist in Linux?"

Not:
    "Which devices are connected?"

Example meaning:
    bus 1 exists because some controller driver successfully registered adapter #1

Senior mental model:
    adapter list = bus inventory

========================================================================================================
PART 11 — WHAT i2cdetect -y 1 REALLY DOES
========================================================================================================

Command:
    i2cdetect -y 1

This scans bus 1 for addresses that respond.

It answers:
    "Which slave addresses on adapter 1 acknowledge probing?"

If it shows:
    77

that means:
    some physical device at address 0x77 responded on bus 1

Important caution:
    this is still user-space probing through i2c-dev
    it is not the same as a kernel client driver being bound

So detection means:
    "the electrical device seems to be there"

It does not automatically mean:
    "Linux kernel already has the correct client driver attached"

========================================================================================================
PART 12 — DEVICE TREE VIEW
========================================================================================================

Modern embedded Linux typically describes hardware using Device Tree.

For I2C, the device tree has two different conceptual layers:

    1. controller nodes
    2. child device nodes on those controllers

CONTROLLER NODE MEANING
───────────────────────
An I2C controller DT node describes:
    - which SoC I2C hardware block this is
    - register address range
    - compatible string for controller driver matching
    - status = "okay" or "disabled"
    - bus speed, pins, clocks, etc.

Example meaning:

    i2c@...
        compatible = ...
        reg = ...
        clock-frequency = ...
        status = "okay";

If status = "okay":
    kernel is allowed to probe/register this adapter

If status = "disabled":
    this potential controller exists in silicon, but Linux should ignore it

That explains why many I2C nodes may exist in DT, but only some appear as /dev/i2c-*.

CLIENT-DEVICE NODE MEANING
──────────────────────────
Inside an enabled controller node, child nodes can represent slave devices on that bus.

Conceptually:

    i2c controller node
        └── bmp180@77
                reg = <0x77>
                compatible = "..."

This means:
    there is a slave device at address 0x77 on this adapter
    and it should be instantiated automatically at boot

========================================================================================================
PART 13 — BOOT-TIME CREATION FLOW FROM DEVICE TREE
========================================================================================================

This is the real boot flow:

    kernel parses Device Tree
        ↓
    finds I2C controller node with status = "okay"
        ↓
    matching adapter/controller driver is loaded
        ↓
    driver registers i2c_adapter
        ↓
    adapter gets bus number (for example i2c-1)
        ↓
    kernel sees child I2C devices under that controller node
        ↓
    i2c_client objects are created for those addresses
        ↓
    I2C core tries to match each client with registered i2c_driver
        ↓
    matching driver's probe() is called

This is the most important system-level diagram in the lesson.

========================================================================================================
PART 14 — WHY /dev/i2c-1 EXISTS
========================================================================================================

It exists because:

    1. an I2C controller node is enabled in Device Tree
    2. the matching controller/bus driver probed successfully
    3. that bus driver registered an i2c_adapter
    4. i2c-dev exposes adapters as /dev/i2c-*

So:

    enabled DT node
        ↓
    bus driver probe
        ↓
    adapter registration
        ↓
    /dev/i2c-1

Senior takeaway:
    /dev/i2c-1 is evidence of a registered adapter, not of any specific slave client driver

========================================================================================================
PART 15 — SYSFS VIEW OF THE SAME OBJECTS
========================================================================================================

The lesson uses:

    /sys/class/i2c-dev/
    /sys/class/i2c-dev/i2c-1/
    /sys/class/i2c-dev/i2c-1/device/

These are different windows into the same kernel object model.

Mental map:

    /sys/class/i2c-dev/
        shows adapters exposed through i2c-dev class

    /sys/class/i2c-dev/i2c-1/
        one specific adapter class entry

    /sys/class/i2c-dev/i2c-1/device/
        underlying kernel device for that adapter
        plus control files like new_device/delete_device

So sysfs gives you the kernel-device-model perspective,
while /dev gives you the file-interface perspective.

========================================================================================================
PART 16 — DYNAMIC CLIENT CREATION THROUGH SYSFS
========================================================================================================

Command:
    echo eeprom 0x50 > /sys/class/i2c-dev/i2c-1/device/new_device

This is extremely important conceptually.

What it means:
    "Create an I2C client instance on adapter 1 at address 0x50 and name/type it as eeprom"

Kernel flow after this command:

    write to new_device
        ↓
    kernel creates an i2c_client on bus 1 address 0x50
        ↓
    client name / identity = "eeprom"
        ↓
    I2C core searches registered i2c_driver objects for a match
        ↓
    if a matching driver exists:
           probe(client) is called

So this sysfs interface is a manual way to instantiate a client device at runtime.

Important detail:
    "eeprom" must match something the driver knows, typically via i2c_device_id table

So the real meaning is not just:
    "talk to address 0x50"

It is:
    "materialize a Linux client object and try to bind a driver to it"

========================================================================================================
PART 17 — WHAT 1-0050 MEANS IN /sys/bus/i2c/devices
========================================================================================================

After dynamic creation, sysfs may show:

    1-0050

This name means:

    bus number 1
    slave address 0x50

This is the concrete i2c_client instance identity.

So sysfs naming convention encodes:

    <adapter number>-<slave address>

That is the kernel object representing the physical EEPROM-like device instance.

========================================================================================================
PART 18 — DYNAMIC CLIENT DELETION
========================================================================================================

Command:
    echo 0x50 > /sys/class/i2c-dev/i2c-1/device/delete_device

Meaning:
    remove the manually created client at address 0x50 from adapter 1

Kernel flow:

    delete request
        ↓
    client object is removed
        ↓
    if a driver was bound:
           remove() is called
        ↓
    sysfs entry disappears

So dynamic client creation/removal is a runtime binding exercise.

Senior takeaway:
    new_device/delete_device is a manual instantiation mechanism for testing and bring-up,
    very useful before fully describing hardware in Device Tree

========================================================================================================
PART 19 — USER SPACE vs KERNEL SPACE PATHS
========================================================================================================

There are two different paths into the same subsystem.

PATH A — USER SPACE DIRECT BUS ACCESS
─────────────────────────────────────
    user app / i2c-tools
        ↓
    /dev/i2c-1
        ↓
    i2c-dev
        ↓
    I2C core
        ↓
    adapter
        ↓
    physical bus transaction

This path is generic and bus-oriented.

PATH B — KERNEL DRIVER DEVICE ACCESS
────────────────────────────────────
    i2c_driver
        ↓
    I2C core
        ↓
    matched i2c_client
        ↓
    adapter
        ↓
    physical bus transaction

This path is device-oriented and integrated into kernel lifecycle.

Both use the same underlying I2C subsystem.
The difference is where the request originates and how the device is modeled.

========================================================================================================
PART 20 — ROLE OF i2c_transfer() AND RELATED APIS
========================================================================================================

The lesson mentions common APIs such as i2c_transfer().

Senior mental model:

    i2c_transfer()
        is a core bus-transaction API
        used by kernel-side drivers to request actual bus transfers

That means a client driver usually does not drive GPIOs or clocks directly.
Instead it says, effectively:

    "Core, please perform this I2C transaction on this adapter/client"

So again:
    bus mechanics belong to adapter/algorithm side
    device semantics belong to client driver side

========================================================================================================
PART 21 — COMPLETE END-TO-END FLOW FOR A REAL SENSOR
========================================================================================================

Take BMP180 at address 0x77 on bus 1.

The full system story is:

    physical BMP180 connected to Raspberry Pi I2C pins
        ↓
    Raspberry Pi DT enables I2C controller node
        ↓
    Broadcom I2C controller driver probes
        ↓
    i2c_adapter #1 registered
        ↓
    /dev/i2c-1 appears
        ↓
    user can verify bus with i2cdetect -l
        ↓
    user can probe addresses with i2cdetect -y 1
        ↓
    address 0x77 responds
        ↓
    either:
         a) user-space tools access it via i2c-dev
       or
         b) kernel creates i2c_client for 0x77 and binds bmp180 i2c_driver
        ↓
    bmp180 driver's probe() runs
        ↓
    driver performs device-specific register reads/writes through I2C core
        ↓
    sensor becomes usable through appropriate kernel subsystem interface

That is the full architecture path.

========================================================================================================
PART 22 — WHAT MATTERS MOST AS A CLIENT-DRIVER AUTHOR
========================================================================================================

As a client-driver developer, the most important ideas are:

    1. You are not writing the controller/bus driver
       You assume adapter already exists.

    2. Your device is represented by i2c_client
       That is your concrete hardware instance.

    3. Your logic lives in i2c_driver
       Probe/remove/match tables belong there.

    4. Binding is done by I2C core
       Based on device identity information.

    5. User-space detection and kernel driver binding are different things
       i2cdetect proves bus/device presence, not full driver integration.

========================================================================================================
PART 23 — ONE-LINE MEANINGS OF THE MOST IMPORTANT OBJECTS
========================================================================================================

i2c_adapter
    = one Linux-visible I2C bus controller instance

i2c_algorithm
    = transfer implementation rules for that adapter

i2c_client
    = one slave device instance on one adapter at one address

i2c_driver
    = kernel logic that knows how to operate that device type

i2c core
    = registration + matching + common bus API infrastructure

i2c-dev
    = generic user-space bridge to adapters

/dev/i2c-1
    = user-space file handle for adapter 1, not for one specific sensor

1-0050
    = kernel sysfs name for client at bus 1, address 0x50

new_device
    = manual runtime client creation entry

delete_device
    = manual runtime client removal entry

========================================================================================================
FINAL SENIOR TAKEAWAY
========================================================================================================

The Linux I2C subsystem is best understood as a bus framework with one core matchmaker:

    controller hardware
        ↓
    i2c_adapter + i2c_algorithm
        ↓
    I2C core
        ↓
    i2c_client at address X
        ↓
    matching i2c_driver
        ↓
    probe()

And the same subsystem is exposed in two parallel views:

    user-space bus view:
        /dev/i2c-* via i2c-dev

    kernel device-driver view:
        i2c_client / i2c_driver via I2C core

So the true mental model is:

    I2C bus exists as an adapter,
    slave chip exists as a client,
    driver logic exists as an i2c_driver,
    and the I2C core is what binds everything together.

========================================================================================================

```