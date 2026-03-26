```
====================================================================================================
LINUX I2C SUBSYSTEM — COMPLETE SENIOR MENTAL MODEL
====================================================================================================

KEY QUESTION THIS LESSON ANSWERS
----------------------------------------------------------------------------------------------------
When a physical I2C sensor is wired to a Linux board:

    how does that hardware become a kernel object
    and how does a driver finally get its probe() called?

To answer this, we must understand:

    Physical Bus
    → Controller Hardware
    → Linux Adapter Object
    → Client Device Object
    → Driver Object
    → Probe()

This entire chain is managed by the **I2C core**.

====================================================================================================
LAYER 0 — PHYSICAL BUS
====================================================================================================

Hardware topology:

        VCC
         │
      [pull-up]
         │
SDA  ────┼──────────────────────────────────────── shared data line
         │         │             │             │
         │         │             │             │
      master     slave A       slave B       slave C

        VCC
         │
      [pull-up]
         │
SCL  ────┼──────────────────────────────────────── shared clock line
         │         │             │             │
         │         │             │             │
      master     slave A       slave B       slave C

Important properties:

• multi-device shared bus
• master generates clock
• slaves respond to addresses
• open-drain signalling

Every device listens to SDA/SCL,
but **only responds when its address appears**.

Senior mental model:

    I2C = shared address-based message bus

Not:
    point-to-point connection.

====================================================================================================
LAYER 1 — CONTROLLER HARDWARE
====================================================================================================

On SoCs (Raspberry Pi, ARM boards, etc), an I2C bus is not created magically.

It is controlled by a **hardware I2C controller block** inside the chip.

Example:

    Broadcom I2C controller
    STM32 I2C controller
    Intel I2C controller

That controller:

    drives SCL
    reads/writes SDA
    generates start/stop conditions
    handles ACK/NACK

Linux must have a **driver for that controller**.

Without that controller driver:

    there is NO I2C bus in Linux.

Senior debugging rule:

If `/dev/i2c-*` does not exist,
the controller driver probably never registered an adapter.

====================================================================================================
LAYER 2 — LINUX REPRESENTS A BUS AS i2c_adapter
====================================================================================================

Kernel object:

    struct i2c_adapter

Represents:

    one I2C controller instance
    exposed as one Linux I2C bus

Diagram:

    Physical Controller Hardware
                │
                ▼
         i2c_adapter object
                │
                ▼
           I2C Core

Example mapping:

    adapter #1
        ↔
    /dev/i2c-1
        ↔
    /sys/class/i2c-dev/i2c-1

Senior mental model:

    adapter = Linux object representing "bus N"

Not a device.
Not a sensor.

It is **the bus itself**.

====================================================================================================
LAYER 3 — HOW THE BUS ACTUALLY PERFORMS TRANSFERS
====================================================================================================

Each adapter contains:

    struct i2c_algorithm

This structure defines **how transfers are executed**.

Example operations inside algorithm:

    master_xfer()
    functionality()

Meaning:

    the adapter does not itself know how to talk I2C
    it delegates transfer logic to the algorithm implementation.

Senior interpretation:

    adapter = bus identity
    algorithm = bus implementation

This separation allows different controllers to reuse the same framework.

Client drivers almost never interact with `i2c_algorithm`.

====================================================================================================
LAYER 4 — DEVICES ON THE BUS (i2c_client)
====================================================================================================

A physical chip on the bus becomes:

    struct i2c_client

This represents:

    one device
    on one bus
    at one address

Identity:

    (adapter, address)

Example:

    adapter 1
    address 0x77

Kernel name:

    1-0077

Meaning:

    bus 1
    address 0x77

Sysfs location:

    /sys/bus/i2c/devices/1-0077

Senior mental model:

    i2c_client = concrete device instance

Example:

    BMP180 sensor at 0x77

Even if two identical sensors exist:

    1-0077
    2-0077

they are different **clients** because they are on different buses.

====================================================================================================
LAYER 5 — DEVICE DRIVER (i2c_driver)
====================================================================================================

The driver describes how to operate that device.

Kernel object:

    struct i2c_driver

Contains:

    name
    id tables
    probe()
    remove()

Example:

    bmp180_i2c_driver

Driver responsibilities:

    configure device
    read registers
    expose functionality to kernel subsystems

Important concept:

    driver does NOT create the device

It only reacts when a matching device appears.

====================================================================================================
LAYER 6 — THE I2C CORE (THE MATCHMAKER)
====================================================================================================

The I2C core manages:

    adapter registration
    driver registration
    client creation
    client-driver matching
    transfer APIs

Architecture diagram:

        Controller Driver
               │
               ▼
        i2c_adapter
               │
               ▼
            I2C Core
               │
      ┌────────┴────────┐
      │                 │
      ▼                 ▼
  i2c_client        i2c_driver
      │                 │
      └───────match─────┘
              │
              ▼
           probe()

Senior mental model:

    I2C core is the operating system for the bus.

It knows:

    which buses exist
    which devices exist
    which drivers exist

and it connects them.

====================================================================================================
BOOT FLOW — HOW A DEVICE BECOMES A DRIVER INSTANCE
====================================================================================================

Full chain:

Device Tree
     │
     ▼
I2C controller node enabled
     │
     ▼
Controller driver probes
     │
     ▼
i2c_adapter registered
     │
     ▼
/dev/i2c-1 created
     │
     ▼
Child device nodes discovered
     │
     ▼
i2c_client objects created
     │
     ▼
I2C core matches driver
     │
     ▼
driver probe() executes

This is the **single most important lifecycle**.

====================================================================================================
DEVICE TREE MODEL
====================================================================================================

Device Tree has two conceptual layers:

1) CONTROLLER NODE

Example:

    i2c@7e804000 {
        status = "okay";
        clock-frequency = <100000>;
    };

Meaning:

    this hardware controller exists
    kernel should enable it.

If status = "disabled":

    adapter will never appear.

2) DEVICE NODE

Example:

    bmp180@77 {
        compatible = "bosch,bmp180";
        reg = <0x77>;
    };

Meaning:

    device exists at address 0x77
    kernel should instantiate client.

====================================================================================================
USER SPACE VIEW
====================================================================================================

User space sees:

    /dev/i2c-1
    /dev/i2c-0

These correspond to adapters.

Important:

    /dev/i2c-1 = bus access point

NOT:

    the sensor itself.

User tools:

    i2cdetect
    i2cget
    i2cset

These tools communicate through:

    i2c-dev driver

Flow:

    userspace
        │
        ▼
    /dev/i2c-1
        │
        ▼
    i2c-dev
        │
        ▼
    I2C core
        │
        ▼
    adapter
        │
        ▼
    physical bus

====================================================================================================
KERNEL DRIVER VIEW
====================================================================================================

Kernel drivers operate differently.

They interact with:

    i2c_client

Flow:

    driver
        │
        ▼
    i2c_client
        │
        ▼
    I2C core
        │
        ▼
    adapter
        │
        ▼
    physical bus

So kernel code never opens `/dev/i2c-1`.

====================================================================================================
DYNAMIC CLIENT CREATION (sysfs)
====================================================================================================

Manual creation:

    echo eeprom 0x50 > /sys/class/i2c-dev/i2c-1/device/new_device

Meaning:

    create i2c_client(bus=1, addr=0x50)

Kernel flow:

    create client
        │
        ▼
    search drivers
        │
        ▼
    match by id table
        │
        ▼
    call probe()

Deletion:

    echo 0x50 > /sys/class/i2c-dev/i2c-1/device/delete_device

Used heavily during bring-up.

====================================================================================================
DEBUGGING HIERARCHY (VERY IMPORTANT)
====================================================================================================

When I2C fails, debug in this order:

1️⃣ Check adapter

    i2cdetect -l

If missing:
    controller driver or DT problem.

2️⃣ Check bus communication

    i2cdetect -y 1

If device not detected:
    wiring / pullups / power / address issue.

3️⃣ Check kernel client

    ls /sys/bus/i2c/devices/

4️⃣ Check driver binding

    dmesg | grep probe

Senior rule:

    always debug bottom-up.

====================================================================================================
FINAL ARCHITECTURE SUMMARY
====================================================================================================

Complete architecture:

    Physical I2C Bus
           │
           ▼
    I2C Controller Hardware
           │
           ▼
    i2c_adapter
           │
           ▼
         I2C Core
           │
     ┌─────┴─────┐
     │           │
     ▼           ▼
 i2c_client   i2c_driver
     │           │
     └────match──┘
           │
           ▼
         probe()

Two parallel system views:

USER SPACE
    /dev/i2c-1
        │
        ▼
    i2c-dev
        │
        ▼
      adapter

KERNEL SPACE

    i2c_driver
        │
        ▼
     i2c_client
        │
        ▼
      adapter

Both paths use the same I2C core.

====================================================================================================
SENIOR ONE-LINE MENTAL MODEL
====================================================================================================

An I2C controller becomes an `i2c_adapter`, a physical chip becomes an `i2c_client`,
a device driver is an `i2c_driver`, and the I2C core binds the client to the driver
and invokes `probe()`.

====================================================================================================
```