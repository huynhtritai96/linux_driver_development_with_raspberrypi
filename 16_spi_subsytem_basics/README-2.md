

```
====================================================================================================
LINUX SPI SUBSYSTEM — SENIOR MENTAL MODEL
====================================================================================================

[ PHYSICAL WORLD ]
----------------------------------------------------------------------------------------------------
Raspberry Pi SPI0 controller
    ├── CS0  ---------------------->  SPI slave device on spi0.0
    └── CS1  ---------------------->  SPI slave device on spi0.1

Signals:
    SCLK   = clock from master
    MOSI   = master -> slave data
    MISO   = slave -> master data
    CS     = chip select, chooses ONE slave

COMMENT:
- SPI does not use addresses like I2C.
- The slave is selected by hardware chip-select.
- So "which device am I talking to?" is decided by CS line, not by an address byte.

----------------------------------------------------------------------------------------------------
[ KERNEL HARDWARE DRIVER LAYER ]
----------------------------------------------------------------------------------------------------
spi-bcm2835.c   <-- controller/master driver for Raspberry Pi SoC SPI hardware
    |
    | owns knowledge of:
    |   - hardware registers
    |   - FIFO
    |   - clock divider
    |   - chip select handling
    |   - polling / IRQ / DMA path
    v
struct spi_controller

COMMENT:
- This layer knows HOW to shift bits on the wire.
- It does NOT know what the external device means.
- It only knows how to make SPI transactions happen electrically.

----------------------------------------------------------------------------------------------------
[ SPI CORE ]
----------------------------------------------------------------------------------------------------
spi.c
    |
    | manages:
    |   - spi_controller registration
    |   - spi_device registration
    |   - spi_driver registration
    |   - matching device <-> driver
    |   - spi_message queueing
    |   - spi_sync / spi_async flow
    v
generic SPI bus framework

COMMENT:
- SPI core is the bus manager.
- It is the middle layer between: "hardware-specific controller driver" and "device-specific client driver".
- This is why Linux drivers stay modular.

----------------------------------------------------------------------------------------------------
[ DEVICE / CLIENT DRIVER LAYER ]
----------------------------------------------------------------------------------------------------
spidev.c              <-- generic userspace SPI bridge
my_spi_driver         <-- your custom SPI client driver
other device drivers  <-- display, ADC, sensor, flash, etc.

Each SPI client driver knows:
    - what command bytes to send
    - what returned bytes mean
    - how to initialize that device
    - what protocol the device expects

COMMENT:
- This layer knows WHAT bits mean.
- The controller driver knows HOW to send bits.
- This split is one of the most important design ideas in Linux.

====================================================================================================
BOOT / REGISTRATION FLOW
====================================================================================================

1. Device Tree says:
       "There is an SPI controller here"
       compatible = "brcm,bcm2835-spi"
       status = "okay"

2. Kernel matches that with:
       spi-bcm2835 driver

3. spi-bcm2835 probe runs
       -> controller hardware initialized
       -> struct spi_controller registered

4. SPI devices exist under that controller
       example:
           spidev@0   -> chip select 0
           spidev@1   -> chip select 1

5. SPI core creates device objects:
       spi0.0
       spi0.1

6. Matching happens:
       spi_device  <->  spi_driver

7. If matched:
       driver probe() is called

COMMENT:
- First the BUS must exist.
- Then DEVICES on that bus can exist.
- Then DRIVERS can bind to those devices.
- This dependency order is critical:
      no controller -> no spi_device -> no probe()

====================================================================================================
OBJECT MODEL
====================================================================================================

struct spi_controller = one SPI bus controller hardware instance
struct spi_device = one slave device on one chip-select of one controller
struct spi_driver = one driver that can bind to matching spi_device objects
struct spi_transfer = one transfer segment (one TX/RX chunk)
struct spi_message = one whole SPI transaction made of one or more spi_transfer objects

COMMENT:
Think about it like this:
    controller  = bus hardware
    device      = one slave on that bus
    driver      = software for that slave
    transfer    = one packet chunk
    message     = one complete transaction

Very often beginners confuse:
    spi_device  vs  spi_driver
So remember:
    spi_device  = the thing that exists in the system
    spi_driver  = the code that knows how to use it

====================================================================================================
WHY /dev/spidev0.0 EXISTS
====================================================================================================
Device Tree / kernel creates spi0.0
        |
        v
spidev driver binds to it
        |
        v
spidev creates /dev/spidev0.0

COMMENT:
- /dev/spidev0.0 is not "SPI hardware itself".
- It is a userspace interface created by the spidev client driver.
- So:
      spi-bcm2835 creates the bus capability
      spidev creates the userspace file node

This distinction matters a lot.

====================================================================================================
YOUR CUSTOM DRIVER REGISTRATION
====================================================================================================

module_spi_driver(my_spi_driver)
    expands to:
        init  -> spi_register_driver(&my_spi_driver)
        exit  -> spi_unregister_driver(&my_spi_driver)

my_spi_driver contains:
    .driver.name = "my_spi_driver"
    .probe       = my_probe
    .remove      = my_remove
    .id_table    = my_ids

COMMENT:
- Registering the driver only means: "I am available if a matching SPI device appears."
- It does NOT guarantee probe() will run.
- probe() runs only when a concrete spi_device matches this driver.

====================================================================================================
WHY PROBE DID NOT RUN AT FIRST
====================================================================================================

Your id_table says:
    "mydevice"

But existing device was:
    spi0.0 currently bound to spidev

So two problems existed:
1. spi0.0 was already owned by another driver
2. spi0.0 did not naturally match your id_table

That is why probe() did not run.

COMMENT:
This is the most important debugging habit:
Whenever probe() does not run, ask:
    A. does the device object exist?
    B. is it already bound to another driver?
    C. does the device identity actually match my driver table?
    D. did I force override or not?

====================================================================================================
MANUAL UNBIND / OVERRIDE / BIND FLOW
====================================================================================================

Initial state:
    spi0.0  --->  spidev

Step 1: unbind from spidev
    echo spi0.0 > /sys/bus/spi/drivers/spidev/unbind

Now:
    spi0.0 is free

Step 2: force driver override
    echo my_spi_driver > /sys/bus/spi/devices/spi0.0/driver_override

Meaning:
    "Ignore normal matching rules; prefer my_spi_driver for this device."

Step 3: bind
    echo spi0.0 > /sys/bus/spi/drivers/my_spi_driver/bind

Now:
    SPI core binds spi0.0 to my_spi_driver
    -> my_probe() runs

COMMENT:
- override is a debug / learning tool here.
- It is not the normal production method.
- In real systems, matching is usually done by Device Tree compatible data or proper device naming.

====================================================================================================
PROBE FUNCTION — WHAT REALLY HAPPENS
====================================================================================================

my_probe(struct spi_device *spi)
    |
    | configure SPI device parameters
    |   spi->mode = SPI_MODE_0
    |   spi->max_speed_hz = 1000000
    |   spi_setup(spi)
    |
    | prepare TX and RX buffers
    |   tx = 0x12
    |   rx = 0x00
    |
    | prepare one spi_transfer
    |   .tx_buf = &tx
    |   .rx_buf = &rx
    |   .len    = 1
    |
    | prepare one spi_message
    |   spi_message_init(&m)
    |   spi_message_add_tail(&t, &m)
    |
    | execute synchronous transfer
    |   spi_sync(spi, &m)
    v
transfer completes

COMMENT:
This is the clean conceptual sequence:

    configure bus behavior
        ->
    describe the bytes to send/receive
        ->
    wrap that into Linux SPI objects
        ->
    ask SPI core to perform the transaction

That is exactly how a client driver should think.

====================================================================================================
FULL DUPLEX EXPLANATION
====================================================================================================

Master sends:
    0x12

Slave sends:
    0x34

Because SPI is full duplex, during the same clock cycles:
    MOSI carries 0x12  ---> slave receives 0x12
    MISO carries 0x34  ---> master receives 0x34

Result:
    tx = 0x12
    rx = 0x34

COMMENT:
This is one of the easiest places to get confused.

SPI is NOT: "first send then later receive"

SPI is:
    "every clock shifts one bit out and one bit in at the same time"

So even when you think "I am writing", you are also receiving something.
And even when you think "I am reading", you must still transmit dummy bytes to generate the clock.

This is a huge conceptual difference from I2C.

====================================================================================================
spi_transfer vs spi_message
====================================================================================================

spi_transfer = one transfer block

Example:
    send 1 byte, receive 1 byte

spi_message = container for one or more transfers

Example:
    message:
        transfer 1 -> send command byte
        transfer 2 -> read response bytes
        transfer 3 -> send configuration data

COMMENT:
A very good mental model is:
    spi_transfer = one segment
    spi_message  = one full conversation

In your demo:
    one message contains one transfer only

But in real drivers:
    one message often contains multiple transfers

====================================================================================================
WHERE spi_sync() GOES
====================================================================================================
my_probe()
    |
    v
spi_sync(spi, &m)
    |
    v
SPI core (spi.c)
    |
    | queue / validate / dispatch
    v
controller driver's transfer callback
    |
    v
spi-bcm2835.c
    |
    | choose transfer method
    |   - polling
    |   - IRQ
    |   - DMA
    v
write controller registers
    |
    v
hardware toggles SCLK, MOSI, MISO, CS
    |
    v
RX buffer filled
    |
    v
return back up stack

COMMENT:
This is the most important vertical flow to remember:

    client driver
        -> SPI core
            -> controller driver
                -> hardware

If you keep this flow in your head, debugging becomes much easier.

====================================================================================================
WHY YOU DID NOT NEED BUS NUMBER OR CHIP SELECT IN YOUR DRIVER
====================================================================================================
Because the device was already:
    spi0.0

That means SPI core already gave your probe function a fully constructed:
    struct spi_device *spi

And inside that object, Linux already knows:
    - which controller
    - which chip select
    - mode / settings context

So your driver just receives the ready-to-use device object.

COMMENT:
As a client driver author, you usually do NOT create bus topology yourself.
You operate on the spi_device passed to you.
That is why probe() is so important: it is the point where Linux hands you the specific hardware instance.

====================================================================================================
REMOVE FLOW
====================================================================================================
rmmod my_spi
    |
    v
spi_unregister_driver()
    |
    v
device unbound
    |
    v
my_remove() called

COMMENT:
remove() is the mirror of probe()

probe() :acquire / initialize / register runtime things
remove() : stop / release / unregister runtime things

In your tiny demo, remove only prints a message.
In a real driver, this is where you would free resources.

====================================================================================================
WHY AUTO-BIND HAPPENED LATER
====================================================================================================

Because you had already written:
    driver_override = my_spi_driver

That override stayed associated with spi0.0

So next time your module loaded:
    SPI core saw spi0.0 already wants my_spi_driver
    -> automatic match
    -> probe() called immediately

COMMENT:
This is an easy thing to miss.

Binding state is not only about the driver module existing.
The device object itself can carry matching state like:
    driver_override

So Linux remembered your forced preference.

====================================================================================================
HOW TO DEEPLY UNDERSTAND THIS SUBSYSTEM
====================================================================================================

Use these 6 questions every time you read an SPI driver:

1. Who owns the hardware registers?
    -> controller driver

2. Who owns the bus policy and matching?
    -> SPI core

3. Who owns the device-specific protocol?
    -> SPI client driver

4. What is the concrete device instance?
    -> struct spi_device

5. What is the concrete transaction description?
    -> spi_message + spi_transfer

6. What causes probe() to run?
    -> device/driver match

If you can answer those 6 questions, you understand most of the subsystem.

====================================================================================================
BEST SIMPLE MENTAL MODEL
====================================================================================================

PHYSICAL:
    SPI controller + slave + wires

KERNEL OBJECTS:
    spi_controller
        ->
    spi_device
        <->
    spi_driver

DATA FLOW:
    spi_transfer inside spi_message
        executed by
    spi_sync()

CALL PATH:
    client driver
        ->
     SPI core
        ->
     controller driver
        ->
      hardware

LIFECYCLE:
      register driver
        ->
    match device
        ->
      probe
        ->
      transfer
        ->
      remove
        ->
     unregister

====================================================================================================
SENIOR COMMENTS TO HELP YOU REMEMBER
====================================================================================================

1. **SPI core is not the hardware driver**
It is the traffic manager.  
This is the biggest misunderstanding many beginners have.

2. **spi_device is not your driver**
It is the kernel's object representing one slave device instance.

3. **spidev is just one client driver**
It is not the SPI subsystem itself.
It is only a generic bridge to user space.

4. **probe means “binding happened”**
Not “module loaded”. A module can be loaded with zero probes.

5. **SPI is always simultaneously TX and RX**
That is why Linux always thinks in terms of transfer buffers, not just pure write-only operations.

6. **Device Tree normally decides what binds**
Manual bind/unbind/override is great for learning, debugging, and experiments — not the final production path.

====================================================================================================

```