



```
====================================================================================================
BMP180 I2C CLIENT DRIVER — SENIOR MENTAL MODEL
====================================================================================================

[ PHYSICAL WORLD ]
----------------------------------------------------------------------------------------------------
Raspberry Pi I2C controller
        |
        |  shared I2C bus: SDA + SCL
        |
        +-------------------------------> BMP180 sensor @ address 0x77

Important hardware facts
------------------------
bus         = i2c-1
slave addr  = 0x77
chip-id reg = 0xD0
chip-id val = 0x55

COMMENT:
- Physically, BMP180 is just a slave hanging on the shared I2C bus.
- Linux does not think in terms of “just wires”.
- Linux turns this physical connection into structured kernel objects.

====================================================================================================
[ KERNEL OBJECT MODEL ]
====================================================================================================

physical controller hardware
        |
        v
struct i2c_adapter
        |
        v
struct i2c_client
        |
        v
struct i2c_driver
        |
        v
probe()
        |
        v
sysfs files:
    temp_input
    pressure_input

COMMENT:
This is the most important object chain.

- i2c_adapter = one I2C bus instance
- i2c_client  = one concrete slave device on that bus
- i2c_driver  = software that knows how to use that slave

Very often beginners confuse:
    i2c_client  vs  i2c_driver

Remember:
    i2c_client = device object
    i2c_driver = driver code

====================================================================================================
[ STAGE 1 — DRIVER REGISTRATION ONLY ]
====================================================================================================

You define:
    static struct i2c_driver my_driver;
    static struct i2c_device_id my_ids[];

And callbacks:
    my_probe()
    my_remove()

Meaning:
    my_driver = "I am a driver available for matching"
    my_ids    = "These are the device names I support"

Example id table:
    "bmp180-a" -> profile A
    "bmp180-b" -> profile B

COMMENT:
- Registering a driver does NOT create a device.
- It only tells I2C core:
      "If such a device appears, I can handle it."

This is the same debugging rule as many other subsystems:
    module loaded != probe called

====================================================================================================
[ i2c_device_id TABLE ]
====================================================================================================

Example:
    static struct i2c_device_id my_ids[] = {
        { "bmp180-a", (kernel_ulong_t)&a },
        { "bmp180-b", (kernel_ulong_t)&b },
        { }
    };

Each entry contains:
    name
    driver_data

Meaning:
    name        = match key
    driver_data = private per-variant metadata

Flow in probe:
    id   = i2c_client_get_device_id(client)
    data = (struct my_data *)id->driver_data

So matching gives you:
    matched device name
        ->
    associated driver-private profile

COMMENT:
Think of this as a tiny dispatch table:

    if matched name is bmp180-a
        use metadata A

    if matched name is bmp180-b
        use metadata B

So i2c_device_id is not just “string matching”.
It is also a lightweight variant-selection mechanism.

====================================================================================================
[ MODULE_DEVICE_TABLE ]
====================================================================================================

    MODULE_DEVICE_TABLE(i2c, my_ids);

Meaning:
    export supported device identities for module/kernel matching metadata

COMMENT:
- This is not runtime behavior.
- It is declaration metadata.
- It tells the kernel/module ecosystem which identities this driver supports.

====================================================================================================
[ SIMPLE PROBE — FIRST REAL VALIDATION ]
====================================================================================================

my_probe(client)
    |
    | get matched id
    | print matched name
    | recover driver_data profile
    | print profile fields
    | read chip-id register 0xD0
    | print result
    v
return 0

Expected chip-id flow:
    i2c_smbus_read_byte_data(client, 0xD0)
        ->
    returns 0x55

Meaning:
    bus communication works
    address is correct
    sensor responds
    probe path is real

COMMENT:
This first probe proves four things at once:

1. device/driver matching works
2. correct id-table entry was selected
3. driver_data mapping works
4. real I2C communication works

That is why chip-id checking is such a common first probe step.

====================================================================================================
[ WHAT i2c_smbus_read_byte_data() REALLY DOES ]
====================================================================================================

    i2c_smbus_read_byte_data(client, 0xD0)

Mental transaction:
    write register address 0xD0
        then
    read one byte back

COMMENT:
- Your driver is NOT toggling SDA/SCL directly.
- It is asking I2C core/helper APIs to perform a standard register transaction.
- This is the correct layering.

So the mental model is:

    device driver
        asks
    I2C subsystem
        to execute
    a standard bus transaction

====================================================================================================
[ DRIVER REGISTRATION PATH ]
====================================================================================================

module init
    |
    v
i2c_add_driver(&my_driver)
    |
    v
driver appears in:
    /sys/bus/i2c/drivers/

But:
    no client may exist yet
    so probe() may not run

COMMENT:
This distinction is critical:

    driver object exists
    device object may not exist

You need BOTH for probe():
    i2c_driver + i2c_client

====================================================================================================
[ MANUAL CLIENT CREATION THROUGH SYSFS ]
====================================================================================================

Command:
    echo bmp180-a 0x77 > /sys/class/i2c-dev/i2c-1/device/new_device

Kernel flow:
    sysfs new_device write
        |
        v
    create i2c_client(bus=1, addr=0x77, name="bmp180-a")
        |
        v
    I2C core matches name against my_ids[]
        |
        v
    match found
        |
        v
    my_probe(client) runs

Result:
    driver binds successfully

COMMENT:
This is the first full bind path:
    create client
        ->
    match driver
        ->
    probe

It is very useful for learning because it makes the binding process visible.

====================================================================================================
[ WHY i2cdetect SHOWS "UU" ]
====================================================================================================

Before driver binds:
    i2cdetect shows:
        77

Meaning:
    "device responded at 0x77 and is free for probing"

After driver binds:
    i2cdetect shows:
        UU

Meaning:
    "this address is now claimed by a kernel driver"

COMMENT:
This is a great practical signal when debugging:
    77 -> hardware responds
    UU -> kernel owns it now

====================================================================================================
[ WHAT 1-0077 MEANS ]
====================================================================================================

sysfs device name:
    1-0077

Meaning:
    bus     = 1
    address = 0x77

This is the concrete i2c_client object.

COMMENT:
This is not just a random filename.
It encodes the physical location of the device on the I2C topology.

====================================================================================================
[ MANUAL REMOVE PATH ]
====================================================================================================

Command:
    echo 0x77 > /sys/class/i2c-dev/i2c-1/device/delete_device

Kernel flow:
    locate i2c_client on bus 1 addr 0x77
        |
        v
    unbind driver
        |
        v
    call my_remove(client)
        |
        v
    destroy client object

COMMENT:
This proves:
    probe/remove are client lifecycle callbacks,
    not module lifecycle callbacks.

Very important distinction:
    module init/exit
        manage driver registration

    probe/remove
        manage device instance binding

====================================================================================================
[ AUTOMATIC CLIENT CREATION INSIDE THE MODULE ]
====================================================================================================

New objects:
    static struct i2c_adapter *my_adapter;
    static struct i2c_client  *my_client;
    static struct i2c_board_info my_i2c_device_info = {
        I2C_BOARD_INFO("bmp180-a", 0x77),
    };

Meaning of i2c_board_info:
    "There should be a device named bmp180-a at address 0x77"

COMMENT:
This is not the driver.
This is the description used to instantiate the device object.

So separate these mentally:

    i2c_driver
        = software logic

    i2c_board_info
        = board/device description

====================================================================================================
[ AUTOMATIC CREATION FLOW ]
====================================================================================================

module load
    |
    +--> i2c_add_driver(&my_driver)
    |
    +--> my_adapter = i2c_get_adapter(1)
    |
    +--> my_client = i2c_new_client_device(my_adapter, &my_i2c_device_info)
    |
    +--> i2c_put_adapter(my_adapter)

Detailed flow:
    register driver
        ->
    get adapter 1
        ->
    create i2c_client on bus 1 addr 0x77
        ->
    I2C core matches client against driver
        ->
    probe() called automatically

COMMENT:
This is the in-kernel equivalent of sysfs new_device.

So compare:

    sysfs new_device
        = user-space initiated client creation

    i2c_new_client_device()
        = kernel-initiated client creation

Both end at:
    real i2c_client object
        ->
    match
        ->
    probe

====================================================================================================
[ WHY i2c_get_adapter() / i2c_put_adapter() EXIST ]
====================================================================================================

    i2c_get_adapter(1)
        = get reference to adapter object for bus 1

    i2c_put_adapter(adapter)
        = release that extra reference

COMMENT:
This is reference/lifetime management.

You need the adapter to create the client,
but once client creation is done, you should not keep an unnecessary extra reference.

This is standard kernel object discipline.

====================================================================================================
[ AUTO-UNREGISTER FLOW ]
====================================================================================================

module exit
    |
    +--> i2c_unregister_device(my_client)
    |
    +--> i2c_del_driver(&my_driver)

Meaning:
    destroy client first
        -> remove() runs
    destroy driver second

COMMENT:
Correct order is:

    create:
        driver -> adapter ref -> client

    destroy:
        client -> driver

Senior rule:
    acquire in dependency order
    release in reverse dependency order

====================================================================================================
[ NOW THE REAL SENSOR DRIVER STARTS ]
====================================================================================================

The final driver adds real sensor logic:

    struct bmp_calib
        = calibration coefficients from EEPROM

    struct bmp_client
        = per-device runtime state
            - client pointer
            - calibration data
            - oversample setting

This is the production-style direction:
    concrete device object
        ->
    attach driver-private per-device state
        ->
    use it everywhere later

COMMENT:
This is where the demo becomes a real driver.
Now the driver is no longer just matching and printing.
It owns device state and exposes real functionality.

====================================================================================================
[ WHY CALIBRATION EXISTS ]
====================================================================================================

BMP180 gives:
    raw uncompensated temperature
    raw uncompensated pressure

BMP180 also stores:
    11 calibration coefficients in EEPROM

Datasheet says:
    read calibration once
    use it to compensate raw values

So actual data path is:

    calibration EEPROM
        +
    raw measurements
        ->
    compensation formula
        ->
    final human values

COMMENT:
This is the classic sensor-driver pattern:

    transport raw bytes
        ->
    decode
        ->
    calibrate / compensate
        ->
    export engineering units

Without calibration, your values are not useful yet.

====================================================================================================
[ bmp_read_block() HELPER ]
====================================================================================================

    bmp_read_block(client, start_reg, buf, len)
        ->
    i2c_smbus_read_i2c_block_data(...)

Meaning:
    read several consecutive registers at once

COMMENT:
This helper is good design because it centralizes:
    transfer call
    error handling
    block-read behavior

So the driver stays cleaner.

====================================================================================================
[ CALIBRATION READ FLOW ]
====================================================================================================

bmp_read_calib(client, &calib)
    |
    +--> read 22 bytes from 0xAA
    |
    +--> combine bytes into:
            AC1 AC2 AC3 AC4 AC5 AC6 B1 B2 MB MC MD
    |
    +--> store inside struct bmp_calib

COMMENT:
The important mental model is:

    sensor stores calibration as raw register bytes
        ->
    driver parses bytes into meaningful typed fields

This is a one-time initialization job in probe().

====================================================================================================
[ RAW TEMPERATURE READ FLOW ]
====================================================================================================

bmp_read_raw_temp()
    |
    +--> write 0x2E to control register 0xF4
    |       -> start temp conversion
    |
    +--> wait 5 ms
    |
    +--> read 2 bytes from 0xF6
    |
    +--> construct UT

COMMENT:
This is exactly the same sequence you tested with userspace I2C tools.
The driver is now doing it inside the kernel instead of a shell command.

So you should always connect:
    userspace prototype transaction
        ->
    final driver implementation

That is a very good engineering workflow.

====================================================================================================
[ RAW PRESSURE READ FLOW ]
====================================================================================================

bmp_read_raw_pres()
    |
    +--> command = 0x34 + (oss << 6)
    |       -> start pressure conversion with chosen oversampling
    |
    +--> wait conversion time depending on oss
    |
    +--> read 3 bytes from 0xF6
    |
    +--> combine into UP

Meaning:
    higher oversampling
        = better resolution
        = longer conversion delay

COMMENT:
This is a classic tradeoff:
    accuracy vs latency

So `oversample` is not just a number.
It is a measurement policy.

====================================================================================================
[ COMPENSATION FUNCTION ]
====================================================================================================

bmp_compensate(calib, oss, UT, UP, &temp_out, &pres_out)

Input:
    calibration coefficients
    raw temp
    raw pressure
    oversampling

Output:
    temperature in tenths of °C
    pressure in Pa

COMMENT:
This function is not "driver magic".
It is just the datasheet math written in C.

That is one of the most important lessons in sensor drivers:
    a lot of code is just faithfully implementing vendor formulas.

====================================================================================================
[ PER-DEVICE DATA OWNERSHIP ]
====================================================================================================

In probe:
    bc = devm_kzalloc(&client->dev, sizeof(*bc), GFP_KERNEL);
    bc->client = client;
    bc->oversample = 3;
    i2c_set_clientdata(client, bc);

Later:
    bc = i2c_get_clientdata(client);

Meaning:
    allocate state once in probe
    attach it to the i2c_client
    recover it later anywhere using the same client

COMMENT:
This is the I2C equivalent of:
    file->private_data
in character drivers.

Very important pattern:
    kernel object carries driver-private per-instance state

====================================================================================================
[ WHY devm_kzalloc() IS NICE ]
====================================================================================================

Without devm_:
    allocate in probe
    manually free in remove

With devm_:
    memory is tied to device lifetime
    automatically released when device goes away

COMMENT:
devm_ APIs reduce cleanup code and leak risk.

Senior rule:
    if a resource is naturally owned by struct device lifetime,
    consider managed allocation APIs.

====================================================================================================
[ SYSFS EXPORT MODEL ]
====================================================================================================

Driver creates:
    temp_input
    pressure_input

Using:
    DEVICE_ATTR_RO(temp_input)
    DEVICE_ATTR_RO(pressure_input)

Then:
    sysfs_create_group(&client->dev.kobj, &bmp_group)

Result:
    /sys/bus/i2c/devices/1-0077/temp_input
    /sys/bus/i2c/devices/1-0077/pressure_input

COMMENT:
This is how driver data becomes normal files.

That is the Linux design beauty:
    internal driver operation
        ->
    simple text files visible to user space

====================================================================================================
[ WHY _show FUNCTIONS LOOK SPECIAL ]
====================================================================================================

Reading temp_input file causes:
    temp_input_show(dev, attr, buf)

Reading pressure_input file causes:
    pressure_input_show(dev, attr, buf)

Inside show():
    1. convert dev -> i2c_client
    2. recover bmp_client private state
    3. read raw sensor values
    4. compensate
    5. write result as text into buf

COMMENT:
sysfs is basically a tiny file-based RPC mechanism.
Reading a file triggers a driver callback.

So:
    cat temp_input
is not just "reading cached memory"
it can trigger live hardware access.

====================================================================================================
[ COMPLETE READ PATH FOR temp_input ]
====================================================================================================

user:
    cat /sys/bus/i2c/devices/1-0077/temp_input
        |
        v
VFS/sysfs
        |
        v
temp_input_show()
        |
        v
to_i2c_client(dev)
        |
        v
i2c_get_clientdata(client)
        |
        v
bmp_read_raw_temp()
        |
        v
bmp_read_raw_pres()
        |
        v
bmp_compensate()
        |
        v
scnprintf(buf, ..., temp10)
        |
        v
user receives text:
    "200\n"

Meaning:
    200 = 20.0 °C

COMMENT:
This is the full stack:
    file read
        ->
    sysfs callback
        ->
    I2C transactions
        ->
    sensor math
        ->
    text output

That is a beautiful kernel-driver-to-userspace path to understand deeply.

====================================================================================================
[ COMPLETE READ PATH FOR pressure_input ]
====================================================================================================

user:
    cat /sys/bus/i2c/devices/1-0077/pressure_input
        |
        v
pressure_input_show()
        |
        v
read raw values
        |
        v
compensate
        |
        v
return pressure text in Pa

COMMENT:
Both files trigger live sensor reads.
They are not mere static attributes.

====================================================================================================
[ WHY PRESSURE PATH STILL READS TEMPERATURE ]
====================================================================================================

Because BMP180 pressure compensation uses temperature-derived intermediate values.

So pressure calculation depends on temp conversion too.

COMMENT:
This is not a Linux framework quirk.
It is sensor physics / device algorithm from the datasheet.

====================================================================================================
[ REMOVE PATH OF THE REAL DRIVER ]
====================================================================================================

bmp180_remove(client)
    |
    +--> sysfs_remove_group(&client->dev.kobj, &bmp_group)
    |
    +--> dev_info("removed device")

COMMENT:
Because devm_kzalloc was used,
you do NOT need manual free of bmp_client state here.

So remove() mainly undoes:
    externally visible registrations
like sysfs entries.

====================================================================================================
[ COMPLETE LIFECYCLE ]
====================================================================================================

MODULE LOAD
    |
    +--> i2c_add_driver(&bmp180_driver)
    |
    +--> i2c_get_adapter(1)
    |
    +--> i2c_new_client_device(adapter, board_info{name="bmp180", addr=0x77})
    |
    +--> I2C core matches client to driver
    |
    +--> bmp180_probe(client)

PROBE
    |
    +--> allocate per-device state
    |
    +--> attach state to client
    |
    +--> read calibration EEPROM
    |
    +--> create sysfs files
    |
    +--> device ready

RUNTIME
    |
    +--> user reads temp_input / pressure_input
    |
    +--> driver performs I2C transactions
    |
    +--> driver computes values
    |
    +--> user sees engineering units

MODULE UNLOAD
    |
    +--> i2c_unregister_device(client)
    |
    +--> bmp180_remove(client)
    |
    +--> sysfs group removed
    |
    +--> i2c_del_driver(driver)

COMMENT:
This is the full driver lifecycle.
If you can mentally replay this sequence, you understand the driver.

====================================================================================================
[ USER-SPACE TEST PROGRAM ]
====================================================================================================

The test app:
    reads temp_input
    reads pressure_input
    parses integer text
    prints human-readable values

Important:
    it is NOT talking directly to BMP180 over I2C

Instead:
    app
        ->
    sysfs file
        ->
    driver callback
        ->
    I2C core
        ->
    BMP180

COMMENT:
So the user app validates the driver interface,
not the raw I2C bus directly.

====================================================================================================
[ WHY AUTO-CREATION INSIDE MODULE IS ONLY FOR DEMO ]
====================================================================================================

This demo uses:
    i2c_new_client_device()

That is useful for:
    learning
    testing
    bring-up

But in production, clients usually come from:
    Device Tree
    ACPI
    platform/board description

COMMENT:
A real driver usually does NOT invent its own device instance at runtime.
It binds to hardware described by the platform firmware / hardware description.

This is a very important production-vs-demo distinction.

====================================================================================================
BEST SIMPLE MENTAL MODEL
====================================================================================================

PHYSICAL:
    BMP180 exists at address 0x77 on bus 1

KERNEL OBJECTS:
    i2c_adapter
        ->
    i2c_client
        <->
    i2c_driver

DRIVER STATE:
    bmp_client attached to i2c_client

MEASUREMENT FLOW:
    read calibration once
        +
    read raw values
        ->
    compensate
        ->
    export sysfs files

USER FLOW:
    read sysfs file
        ->
    callback runs
        ->
    live I2C transaction happens
        ->
    value returned

====================================================================================================
SENIOR COMMENTS TO HELP YOU REMEMBER
====================================================================================================

1. **Driver and device are different**
The driver can exist without the device.
The device can exist only when created/described.

2. **Probe means binding happened**
Not just “module loaded”.

3. **`i2c_client` is the concrete chip instance**
It is the kernel object for “BMP180 at bus 1 address 0x77”.

4. **The datasheet is part of your driver design**
A sensor driver is often:
    transport logic + datasheet math + Linux interface

5. **sysfs files are not passive text**
Reading them can execute real driver code and real bus transactions.

6. **Calibration is part of functionality, not optional decoration**
Without it, raw readings are not meaningful.

====================================================================================================

```