# SYSTEM ARCHITECT ARM CORTEX A

# Bài 3.1 - 3.3: Device Tree, Touchscreen Driver, Sensor Driver

**Biên soạn:** Phạm Văn Vũ  
**Đơn vị:** HALA Academy  
---

## Mục lục

- [SYSTEM ARCHITECT ARM CORTEX A](#system-architect-arm-cortex-a)
- [Bài 3.1 - 3.3: Device Tree, Touchscreen Driver, Sensor Driver](#bài-31---33-device-tree-touchscreen-driver-sensor-driver)
  - [**Đơn vị:** HALA Academy](#đơn-vị-hala-academy)
  - [Mục lục](#mục-lục)
- [Bài 3.1: Device Tree Chuyên Sâu](#bài-31-device-tree-chuyên-sâu)
  - [Mục tiêu bài học](#mục-tiêu-bài-học)
  - [Phần 1: Device Tree Fundamentals](#phần-1-device-tree-fundamentals)
    - [Hình 1: Cấu trúc Device Tree - Orange Pi Zero 3](#hình-1-cấu-trúc-device-tree---orange-pi-zero-3)
    - [Device Tree Format](#device-tree-format)
    - [1.1 DT Compilation Flow](#11-dt-compilation-flow)
    - [1.2 Node Syntax](#12-node-syntax)
    - [1.3 Important Properties](#13-important-properties)
  - [Phần 2: Phandles \& References](#phần-2-phandles--references)
    - [2.1 Phandle là gì?](#21-phandle-là-gì)
      - [Node được reference](#node-được-reference)
      - [Node reference bằng phandle](#node-reference-bằng-phandle)
    - [2.2 Cells Properties](#22-cells-properties)
  - [Phần 3: Interrupts](#phần-3-interrupts)
    - [3.1 Interrupt Controller](#31-interrupt-controller)
    - [3.2 Interrupt Consumer](#32-interrupt-consumer)
    - [3.3 Interrupt Types](#33-interrupt-types)
  - [Phần 4: Pinctrl](#phần-4-pinctrl)
    - [4.1 Pin Controller](#41-pin-controller)
    - [4.2 Pin Consumer](#42-pin-consumer)
  - [Phần 5: Device Tree Overlay](#phần-5-device-tree-overlay)
    - [5.1 Overlay Syntax](#51-overlay-syntax)
    - [5.2 Compile và Apply](#52-compile-và-apply)
  - [Phần 6: Debug Device Tree](#phần-6-debug-device-tree)
    - [6.1 Xem DT trong Linux](#61-xem-dt-trong-linux)
  - [Phần 7: Câu hỏi ôn tập](#phần-7-câu-hỏi-ôn-tập)
  - [Tài liệu tham khảo](#tài-liệu-tham-khảo)
  - [Yêu cầu bài tập](#yêu-cầu-bài-tập)
- [Bài 3.2: Touchscreen Driver](#bài-32-touchscreen-driver)
  - [Mục tiêu bài học](#mục-tiêu-bài-học-1)
  - [Phần 1: Linux Input Subsystem](#phần-1-linux-input-subsystem)
    - [Hình 1: Kiến trúc Input Subsystem](#hình-1-kiến-trúc-input-subsystem)
    - [1.1 Kiến trúc](#11-kiến-trúc)
    - [1.2 Event Types](#12-event-types)
    - [1.3 Touch Events - EV\_ABS](#13-touch-events---ev_abs)
  - [Phần 2: Driver Structure](#phần-2-driver-structure)
    - [2.1 IRQ Handler](#21-irq-handler)
    - [2.2 Probe Function](#22-probe-function)
  - [Phần 3: Device Tree Binding](#phần-3-device-tree-binding)
  - [Phần 4: Debug và Test](#phần-4-debug-và-test)
    - [4.1 evtest](#41-evtest)
    - [4.2 Check driver status](#42-check-driver-status)
  - [Phần 5: Multitouch Support](#phần-5-multitouch-support)
  - [Phần 6: Câu hỏi ôn tập](#phần-6-câu-hỏi-ôn-tập)
  - [Tài liệu tham khảo](#tài-liệu-tham-khảo-1)
  - [Yêu cầu bài tập](#yêu-cầu-bài-tập-1)
- [Bài 3.3: Sensor Driver - Regmap \& IIO](#bài-33-sensor-driver---regmap--iio)
  - [Mục tiêu bài học](#mục-tiêu-bài-học-2)
  - [Phần 1: IIO Subsystem](#phần-1-iio-subsystem)
    - [Hình 1: Kiến trúc IIO Subsystem](#hình-1-kiến-trúc-iio-subsystem)
    - [1.1 IIO là gì?](#11-iio-là-gì)
    - [1.2 IIO Channel Types](#12-iio-channel-types)
  - [Phần 2: Regmap API](#phần-2-regmap-api)
    - [2.1 Tại sao dùng Regmap?](#21-tại-sao-dùng-regmap)
    - [2.2 Regmap Config](#22-regmap-config)
    - [2.3 Regmap Operations](#23-regmap-operations)
  - [Phần 3: IIO Driver Structure](#phần-3-iio-driver-structure)
    - [3.1 IIO Channel Definition](#31-iio-channel-definition)
    - [3.2 Read Function](#32-read-function)
    - [3.3 Probe Function](#33-probe-function)
  - [Phần 4: Sysfs Interface](#phần-4-sysfs-interface)
  - [Phần 5: Device Tree](#phần-5-device-tree)
  - [Phần 6: Câu hỏi ôn tập](#phần-6-câu-hỏi-ôn-tập-1)
  - [Tài liệu tham khảo](#tài-liệu-tham-khảo-2)
  - [Yêu cầu bài tập](#yêu-cầu-bài-tập-2)
- [Appendix: Tóm tắt luồng dữ liệu quan trọng](#appendix-tóm-tắt-luồng-dữ-liệu-quan-trọng)
  - [Device Tree matching flow](#device-tree-matching-flow)
  - [Touchscreen driver event flow](#touchscreen-driver-event-flow)
  - [IIO sensor read flow](#iio-sensor-read-flow)

---

# Bài 3.1: Device Tree Chuyên Sâu

## Mục tiêu bài học

Sau buổi học này, học viên sẽ có khả năng:

- Hiểu sâu cấu trúc và cú pháp **Device Tree**.
- Nắm vững **Bindings**, **Phandles**, **Interrupts**, **Pinctrl**.
- Sử dụng **DT Overlay** để modify hardware runtime.

---

## Phần 1: Device Tree Fundamentals

### Hình 1: Cấu trúc Device Tree - Orange Pi Zero 3

```text
Device Tree Node Structure - Orange Pi Zero 3

/  (Root Node)
|
|-- Properties
|   |-- model = "OrangePi Zero3"
|   |-- compatible = "allwinner,sun50i-h618"
|   |-- #address-cells = <2>
|   `-- #size-cells = <2>
|
|-- cpus
|   |-- #address-cells = <1>
|   |-- #size-cells = <0>
|   `-- cpu@0
|       |-- compatible = "arm,cortex-a53"
|       |-- device_type = "cpu"
|       `-- reg = <0>
|
`-- soc
    |-- compatible = "simple-bus"
    |-- #address-cells = <1>
    |-- #size-cells = <1>
    |
    |-- serial@5000000  (uart0)
    |   |-- compatible = "allwinner,sun50i-h616-uart"
    |   |-- reg = <0x05000000 0x400>
    |   |-- interrupts = <GIC_SPI 0 ...>
    |   |-- clocks = <&ccu CLK_BUS_UART0>
    |   `-- status = "okay"
    |
    |-- i2c@5002000  (i2c0)
    |   |-- compatible = "allwinner,sun50i-h616-i2c"
    |   |-- reg = <0x05002000 0x400>
    |   `-- status = "okay"
    |
    `-- pinctrl@300b000  (pio)
        |-- compatible = "allwinner,sun50i-h616-pinctrl"
        |-- gpio-controller
        `-- #gpio-cells = <3>

Device Tree Format:

node-name@unit-address {
    compatible = "vendor,device";
    reg = <address size>;
    status = "okay";
};
```

### Device Tree Format

```dts
node-name@unit-address {
    compatible = "vendor,device";
    reg = <address size>;
    status = "okay";
};
```

### 1.1 DT Compilation Flow

```text
DTS source  -->  DTC compiler  -->  DTB binary
```

| File type | Meaning | Purpose |
|---|---|---|
| `.dts` | Device Tree Source | File mô tả board cụ thể |
| `.dtsi` | Device Tree Source Include | File dùng chung giữa nhiều board/SoC |
| `.dtb` | Device Tree Blob | Binary được kernel load |

### 1.2 Node Syntax

```dts
label: node-name@unit-address {
    compatible = "vendor,device";
    reg = <address size>;
    property-name = <value>;

    child-node {
        // ...
    };
};
```

### 1.3 Important Properties

| Property | Mô tả | Ví dụ |
|---|---|---|
| `compatible` | Driver matching | `"allwinner,sun50i-h616-uart"` |
| `reg` | Address/size | `<0x05000000 0x400>` |
| `interrupts` | IRQ specification | `<GIC_SPI 0 IRQ_TYPE_LEVEL_HIGH>` |
| `status` | Enable/disable | `"okay"` hoặc `"disabled"` |
| `clocks` | Clock references | `<&ccu CLK_BUS_UART0>` |

---

## Phần 2: Phandles & References

### 2.1 Phandle là gì?

**Phandle** là unique ID để reference node khác. DTC tự động gán phandle.

#### Node được reference

```dts
clk_osc24m: osc24M-clk {
    #clock-cells = <0>;
    compatible = "fixed-clock";
    clock-frequency = <24000000>;
};
```

#### Node reference bằng phandle

```dts
uart0: serial@5000000 {
    clocks = <&clk_osc24m>; // Reference
};
```

### 2.2 Cells Properties

| Property | Mô tả |
|---|---|
| `#address-cells` | Số cells cho address |
| `#size-cells` | Số cells cho size |
| `#clock-cells` | Args cho clock consumer |
| `#interrupt-cells` | Args cho interrupt |
| `#gpio-cells` | Args cho GPIO consumer |

---

## Phần 3: Interrupts

### 3.1 Interrupt Controller

```dts
gic: interrupt-controller@3021000 {
    compatible = "arm,gic-400";
    #interrupt-cells = <3>;
    interrupt-controller;
    reg = <0x03021000 0x1000>,
          <0x03022000 0x2000>;
};
```

### 3.2 Interrupt Consumer

```dts
uart0: serial@5000000 {
    interrupt-parent = <&gic>;
    interrupts = <GIC_SPI 0 IRQ_TYPE_LEVEL_HIGH>;

    // GIC_SPI = 0: Shared Peripheral Interrupt
    // 0 = IRQ number
    // IRQ_TYPE_LEVEL_HIGH = trigger type
};
```

### 3.3 Interrupt Types

| Macro | Value | Mô tả |
|---|---:|---|
| `IRQ_TYPE_EDGE_RISING` | 1 | Rising edge |
| `IRQ_TYPE_EDGE_FALLING` | 2 | Falling edge |
| `IRQ_TYPE_LEVEL_HIGH` | 4 | High level |
| `IRQ_TYPE_LEVEL_LOW` | 8 | Low level |

---

## Phần 4: Pinctrl

### 4.1 Pin Controller

```dts
pio: pinctrl@300b000 {
    compatible = "allwinner,sun50i-h616-pinctrl";
    reg = <0x0300b000 0x400>;
    #gpio-cells = <3>;
    gpio-controller;

    uart0_ph_pins: uart0-ph-pins {
        pins = "PH0", "PH1";
        function = "uart0";
    };
};
```

### 4.2 Pin Consumer

```dts
&uart0 {
    pinctrl-names = "default";
    pinctrl-0 = <&uart0_ph_pins>;
};
```

---

## Phần 5: Device Tree Overlay

### 5.1 Overlay Syntax

```dts
/dts-v1/;
/plugin/;

&{/} {
    // Add new node to root
    my_led {
        compatible = "gpio-leds";
        status = "okay";

        led0 {
            gpios = <&pio 2 13 GPIO_ACTIVE_HIGH>;
            label = "user-led";
        };
    };
};

&uart1 {
    status = "okay"; // Modify existing node
};
```

### 5.2 Compile và Apply

```bash
# Compile overlay
dtc -@ -I dts -O dtb -o my_overlay.dtbo my_overlay.dts

# Apply tại U-Boot
fdt apply my_overlay.dtbo

# Apply tại Linux (configfs)
mkdir /sys/kernel/config/device-tree/overlays/my_overlay
cat my_overlay.dtbo > /sys/kernel/config/device-tree/overlays/my_overlay/dtbo
```

---

## Phần 6: Debug Device Tree

### 6.1 Xem DT trong Linux

```bash
# View tree structure
ls /proc/device-tree/

# Read property
cat /proc/device-tree/model
hexdump -C /proc/device-tree/serial@5000000/reg

# Decompile runtime DT
dtc -I fs /proc/device-tree > runtime.dts
```

---

## Phần 7: Câu hỏi ôn tập

1. Giải thích cấu trúc một node Device Tree.
2. Phandle là gì? Cho ví dụ.
3. `#interrupt-cells = 3` nghĩa là gì?
4. Overlay dùng để làm gì? Cú pháp?
5. Làm sao debug DT trong Linux?

## Tài liệu tham khảo

- Device Tree Specification: <https://www.devicetree.org/>
- Kernel DT Bindings
- Bootlin DT Training

## Yêu cầu bài tập

- Giải thích được cấu trúc một node DT.
- Tạo được DT overlay enable GPIO LED.
- Debug thành công với `/proc/device-tree`.

---

# Bài 3.2: Touchscreen Driver

## Mục tiêu bài học

Sau buổi học này, học viên sẽ có khả năng:

- Hiểu **Linux Input Subsystem**.
- Viết skeleton driver cho **I2C touchscreen**.
- Debug với **evtest** và **libinput**.

---

## Phần 1: Linux Input Subsystem

### Hình 1: Kiến trúc Input Subsystem

```text
Linux Input Subsystem Architecture

+---------------- Userspace -----------------+
|                                            |
|  +-------------+       +----------------+  |
|  | Application |       | evtest         |  |
|  +------+------+       +-------+--------+  |
|         | Use                  | ioctl     |
|         v                      |           |
|  +-------------+               |           |
|  | libinput    |               |           |
|  +------+------+               |           |
+---------|----------------------|-----------+
          | /dev/input/eventX    |
          v                      v
+---------------- Kernel - Input Core -----------------+
|                                                      |
|  +----------------------+  +-----------------------+ |
|  | input_handler: evdev |  | input_handler: mousedev |
|  +----------+-----------+  +----------+------------+ |
|             | Events                  | Events       |
|             v                         v              |
|          +-----------------------------+             | input_register_device() 
|          |          input_dev          |<------------| input_bit(EV_ABS, evbit)
|          +---------+-------------------+             | input_set_abs_params()
|                         ^                            |                       
+-------------------------|----------------------------+
    ^                     ^                     ^
    |input_report_abs()   | input_report_key()  | input_report_rel()
    |                     |                     |
+---------------- Kernel - Drivers ---------------------+
|                                                       |
| +--------------------+ +----------------+ +---------+ |
| | Touchscreen Driver | | Keyboard Driver| | Mouse   | |
| +---------+----------+ +--------+-------+ | Driver  | |
|           ^                     ^         +----+----+ |
+-----------|---------------------|--------------|------+
            |                     |              |  
            | IRQ + I2C           | USB          | USB
+-----------|--------Hardware-----|--------------|------+
|                                                       |
| +----------------+ +--------------+ +-------------+   |
| | I2C Touchscreen| | USB Keyboard | | USB Mouse   |   |
| +----------------+ +--------------+ +-------------+   |
+-------------------------------------------------------+

Driver setup around input_dev:
- input_register_device()
- set_bit(EV_ABS, evbit)
- input_set_abs_params()
```

### 1.1 Kiến trúc

Luồng dữ liệu:

```text
Driver -> input_dev -> evdev handler -> /dev/input/eventX -> Userspace
```

### 1.2 Event Types

| Type | Macro | Mô tả |
|---|---:|---|
| `EV_SYN` | `0x00` | Sync event |
| `EV_KEY` | `0x01` | Key/button |
| `EV_REL` | `0x02` | Relative, ví dụ mouse |
| `EV_ABS` | `0x03` | Absolute, ví dụ touch |

### 1.3 Touch Events - EV_ABS

| Code | Macro | Mô tả |
|---|---|---|
| `0x00` | `ABS_X` | X position |
| `0x01` | `ABS_Y` | Y position |
| `0x18` | `ABS_PRESSURE` | Pressure |
| `0x35` | `ABS_MT_POSITION_X` | Multitouch X |
| `0x36` | `ABS_MT_POSITION_Y` | Multitouch Y |

---

## Phần 2: Driver Structure

### 2.1 IRQ Handler

```c
static irqreturn_t my_touch_irq(int irq, void *dev_id)
{
    struct my_touch_data *data = dev_id;
    u8 buf[6];
    int x, y, touch;

    // Read touch data from I2C
    i2c_smbus_read_i2c_block_data(data->client, 0x00, 6, buf);

    touch = buf[0] & 0x01;
    x = (buf[1] << 8) | buf[2];
    y = (buf[3] << 8) | buf[4];

    if (touch) {
        input_report_abs(data->input, ABS_X, x);
        input_report_abs(data->input, ABS_Y, y);
        input_report_key(data->input, BTN_TOUCH, 1);
    } else {
        input_report_key(data->input, BTN_TOUCH, 0);
    }

    input_sync(data->input);
    return IRQ_HANDLED;
}
```

### 2.2 Probe Function

```c
static int my_touch_probe(struct i2c_client *client)
{
    struct my_touch_data *data;
    struct input_dev *input;

    data = devm_kzalloc(&client->dev, sizeof(*data), GFP_KERNEL);
    input = devm_input_allocate_device(&client->dev);

    data->client = client;
    data->input = input;

    input->name = "My Touchscreen";
    input->id.bustype = BUS_I2C;

    // Set capabilities
    set_bit(EV_ABS, input->evbit);
    set_bit(EV_KEY, input->evbit);
    set_bit(BTN_TOUCH, input->keybit);

    input_set_abs_params(input, ABS_X, 0, 1024, 0, 0);
    input_set_abs_params(input, ABS_Y, 0, 600, 0, 0);

    // Request IRQ & Register
    devm_request_threaded_irq(&client->dev, client->irq,
                              NULL, my_touch_irq, IRQF_ONESHOT,
                              "my_touch", data);

    input_register_device(input);
    return 0;
}
```

---

## Phần 3: Device Tree Binding

```dts
&i2c1 {
    status = "okay";

    touchscreen@38 {
        compatible = "vendor,my-touch";
        reg = <0x38>;
        interrupt-parent = <&pio>;
        interrupts = <7 4 IRQ_TYPE_EDGE_FALLING>; // PH4
        touchscreen-size-x = <1024>;
        touchscreen-size-y = <600>;
    };
};
```

---

## Phần 4: Debug và Test

### 4.1 evtest

```bash
# Install
apt install evtest

# List và monitor devices
evtest

# /dev/input/event0: My Touchscreen
evtest /dev/input/event0

# Event examples:
# Event: type 3 (EV_ABS), code 0 (ABS_X), value 512
# Event: type 3 (EV_ABS), code 1 (ABS_Y), value 300
# Event: type 1 (EV_KEY), code 330 (BTN_TOUCH), value 1
```

### 4.2 Check driver status

```bash
# Check module
lsmod | grep my_touch

# Check input device
cat /proc/bus/input/devices

# Check DT binding
cat /sys/bus/i2c/devices/1-0038/of_node/compatible
```

---

## Phần 5: Multitouch Support

```c
// In probe
input_mt_init_slots(input, MAX_FINGERS, INPUT_MT_DIRECT);

// In IRQ handler
for (i = 0; i < num_fingers; i++) {
    input_mt_slot(input, i);
    input_mt_report_slot_state(input, MT_TOOL_FINGER, touch);

    if (touch) {
        input_report_abs(input, ABS_MT_POSITION_X, x[i]);
        input_report_abs(input, ABS_MT_POSITION_Y, y[i]);
    }
}

input_mt_sync_frame(input);
input_sync(input);
```

---

## Phần 6: Câu hỏi ôn tập

1. Giải thích luồng data từ driver đến userspace.
2. `EV_ABS` và `EV_KEY` khác nhau như thế nào?
3. `input_sync()` có chức năng gì?
4. Làm sao debug với `evtest`?
5. Multitouch Type B protocol là gì?

## Tài liệu tham khảo

- Kernel Input Documentation
- Multi-touch Protocol
- evtest

## Yêu cầu bài tập

- Hiểu flow: `Driver -> input_dev -> evdev -> userspace`.
- Build được skeleton driver module.
- Test với `evtest` thấy events.

---

# Bài 3.3: Sensor Driver - Regmap & IIO

## Mục tiêu bài học

Sau buổi học này, học viên sẽ có khả năng:

- Hiểu **IIO - Industrial I/O Subsystem**.
- Sử dụng **Regmap API** cho I2C/SPI registers.
- Viết **IIO sensor driver**.

---

## Phần 1: IIO Subsystem

### Hình 1: Kiến trúc IIO Subsystem

```text
Linux IIO Subsystem Architecture

                           Linux IIO Subsystem Architecture
================================================================================

+------------------------------- Userspace ---------------------------+
|   +-------------------+                     +-------------------+   |
|   |   Application     |                     |      libiio       |   |
|   +-------------------+                     +-------------------+   |
|            |                                           |            |
|            | cat in_temp_raw                           |            |
|            v                                           |            |
|   +-----------------------------------+                |            |
|   | /sys/bus/iio/devices/iio:device0  |                |            |
|   +-----------------------------------+                |            |
|            ^                                           |            |
|            |                                           v            |
|            | Export                         +------------------+    |
|            |                                | /dev/iio:device0 |    |
|            |                                +------------------+    |
+------------|--------------------------------------------------------+
             |    Exported sysfs attributes:
             |    /sys/bus/iio/devices/iio:device0/
             |    ├── in_temp_raw
             |    ├── in_temp_scale
             |    └── name
             v
+---------------- Kernel - IIO Core ----------------+
|                                                   |
|  +-------------+                                  |
|  | iio_device  |-------------------+              |
|  +------+------+                   |              |
|         | Optional                 |Contains      |
|         v                          v              |
|  +-------------+        +----------------------+  |
|  | iio_buffer  |        | iio_channel          |  |
|  +------+------+        | - type = IIO_TEMP    |  |
|         ^ Trigger       | - info_mask = RAW    |  |
|         |               +----------+-----------+  |
|  +-------------+                   ^              |
|  | iio_trigger |                   |  read_raw()  |
|  +-------------+       +------------------------+ |
+------------------------| Kernel - Driver        |-+
                         | +--------------------+ |
                         | | IIO Sensor Driver  | |
                         | +---------+----------+ |
                         |           | regmap_read() / regmap_bulk_read()
                         |           v            |
                         | +--------------------+ |
                         | | Regmap API         | |
                         | +---------+----------+ |
                         +-----------|------------+
                                     | I2C/SPI
                                     v
                         +------------------------+
                         | Hardware               |
                         | +--------------------+ |
                         | | I2C/SPI Sensor     | |
                         | +--------------------+ |
                         +------------------------+

struct iio_chan_spec {
    .type = IIO_TEMP,
    .info_mask_separate = BIT(IIO_CHAN_INFO_RAW),
};
```

### 1.1 IIO là gì?

- **IIO**: Subsystem cho sensors như ADC, DAC, accelerometer, gyro, light...
- Cung cấp **standard sysfs interface**.
- Có **buffer support** cho continuous sampling.
- Có **trigger mechanism**.

### 1.2 IIO Channel Types

| Type | Macro | Mô tả |
|---|---|---|
| Voltage | `IIO_VOLTAGE` | ADC input |
| Current | `IIO_CURRENT` | Current sensor |
| Accel | `IIO_ACCEL` | Accelerometer |
| Gyro | `IIO_ANGL_VEL` | Gyroscope |
| Light | `IIO_LIGHT` | Ambient light |
| Temp | `IIO_TEMP` | Temperature |

---

## Phần 2: Regmap API

### 2.1 Tại sao dùng Regmap?

- Abstract hóa I2C/SPI register access.
- Có caching và locking built-in.
- Debug qua debugfs.

### 2.2 Regmap Config

```c
static const struct regmap_config my_sensor_regmap_config = {
    .reg_bits = 8,
    .val_bits = 8,
    .max_register = 0xFF,
    .cache_type = REGCACHE_RBTREE,
};

// In probe
struct regmap *regmap;
regmap = devm_regmap_init_i2c(client, &my_sensor_regmap_config);
```

### 2.3 Regmap Operations

```c
// Read single register
int val;
regmap_read(regmap, REG_STATUS, &val);

// Write single register
regmap_write(regmap, REG_CTRL, 0x01);

// Read-modify-write
regmap_update_bits(regmap, REG_CTRL, MASK_ENABLE, MASK_ENABLE);

// Bulk read
u8 buf[6];
regmap_bulk_read(regmap, REG_DATA, buf, 6);
```

---

## Phần 3: IIO Driver Structure

### 3.1 IIO Channel Definition

```c
static const struct iio_chan_spec my_sensor_channels[] = {
    {
        .type = IIO_ACCEL,
        .modified = 1,
        .channel2 = IIO_MOD_X,
        .info_mask_separate = BIT(IIO_CHAN_INFO_RAW),
        .info_mask_shared_by_type = BIT(IIO_CHAN_INFO_SCALE),
        .scan_index = 0,
        .scan_type = {
            .sign = 's',
            .realbits = 16,
            .storagebits = 16,
            .endianness = IIO_LE,
        },
    },
    // Y and Z channels similar...
};
```

### 3.2 Read Function

```c
static int my_sensor_read_raw(struct iio_dev *indio_dev,
                              struct iio_chan_spec const *chan,
                              int *val, int *val2, long mask)
{
    struct my_sensor_data *data = iio_priv(indio_dev);
    __le16 raw;

    switch (mask) {
    case IIO_CHAN_INFO_RAW:
        regmap_bulk_read(data->regmap,
                         REG_ACCEL_X + chan->scan_index * 2,
                         &raw, sizeof(raw));
        *val = sign_extend32(le16_to_cpu(raw), 15);
        return IIO_VAL_INT;

    case IIO_CHAN_INFO_SCALE:
        *val = 0;
        *val2 = 9810; // 0.009810 m/s^2 per LSB
        return IIO_VAL_INT_PLUS_MICRO;

    default:
        return -EINVAL;
    }
}
```

### 3.3 Probe Function

```c
static int my_sensor_probe(struct i2c_client *client)
{
    struct iio_dev *indio_dev;
    struct my_sensor_data *data;

    indio_dev = devm_iio_device_alloc(&client->dev, sizeof(*data));
    data = iio_priv(indio_dev);

    data->regmap = devm_regmap_init_i2c(client, &my_sensor_regmap_config);

    indio_dev->name = "my-sensor";
    indio_dev->modes = INDIO_DIRECT_MODE;
    indio_dev->channels = my_sensor_channels;
    indio_dev->num_channels = ARRAY_SIZE(my_sensor_channels);
    indio_dev->info = &my_sensor_info;

    return devm_iio_device_register(&client->dev, indio_dev);
}
```

---

## Phần 4: Sysfs Interface

```bash
# List IIO devices
ls /sys/bus/iio/devices/

# Read raw values
cat /sys/bus/iio/devices/iio:device0/in_accel_x_raw
cat /sys/bus/iio/devices/iio:device0/in_accel_y_raw
cat /sys/bus/iio/devices/iio:device0/in_accel_z_raw

# Read scale
cat /sys/bus/iio/devices/iio:device0/in_accel_scale

# Calculate real value: raw * scale
```

---

## Phần 5: Device Tree

```dts
&i2c1 {
    status = "okay";

    accelerometer@1d {
        compatible = "vendor,my-sensor";
        reg = <0x1d>;
        interrupt-parent = <&pio>;
        interrupts = <7 5 IRQ_TYPE_EDGE_RISING>; // PH5
    };
};
```

---

## Phần 6: Câu hỏi ôn tập

1. IIO subsystem dùng cho loại device nào?
2. Regmap có những ưu điểm gì?
3. `IIO_CHAN_INFO_RAW` và `IIO_CHAN_INFO_SCALE` khác nhau?
4. Làm sao đọc sensor qua sysfs?
5. `scan_type` trong `iio_chan_spec` có ý nghĩa gì?

## Tài liệu tham khảo

- IIO Documentation
- Regmap API
- IIO Tools - libiio

## Yêu cầu bài tập

- Hiểu IIO channel structure.
- Sử dụng Regmap cho I2C access.
- Đọc sensor qua sysfs interface.

---

# Appendix: Tóm tắt luồng dữ liệu quan trọng

## Device Tree matching flow

```text
Device Tree matching flow

.dts/.dtsi source
        |
        v
+----------------+
| dtc compiler   |
+-------+--------+
        |
        v
+----------------+
| .dtb binary    |
+-------+--------+
        |
        v
+----------------+
| U-Boot loads   |
| DTB            |
+-------+--------+
        |
        v
+----------------+
| Linux kernel   |
| receives DTB   |
+-------+--------+
        |
        v
+----------------+
| of_platform /  |
| driver core    |
+-------+--------+
        |
        v
+----------------+
| Match          |
| compatible     |
| string         |
+-------+--------+
        |
        v
+----------------+
| driver probe() |
+----------------+
```

## Touchscreen driver event flow

```text
Touchscreen driver event flow

I2C Touchscreen
      |
      | Interrupt when touch changes
      v
IRQ Handler
      |
      | Read coordinates/registers
      v
I2C Read
      |
      | returns x, y, touch state
      v
IRQ Handler
      |
      | input_report_abs(ABS_X / ABS_Y)
      | input_report_key(BTN_TOUCH)
      | input_sync()
      v
input_dev
      |
      | Dispatch input event
      v
evdev
      |
      | /dev/input/eventX
      v
Userspace
```

## IIO sensor read flow

```text
IIO sensor read flow

Userspace
   |
   | cat in_accel_x_raw
   v
sysfs: /sys/bus/iio/devices/iio:device0
   |
   | read raw channel
   v
IIO Core
   |
   | read_raw(chan, IIO_CHAN_INFO_RAW)
   v
Sensor Driver
   |
   | regmap_bulk_read()
   v
Regmap
   |
   | I2C/SPI transaction
   v
I2C/SPI Sensor
   |
   | raw register data
   v
Regmap
   |
   | raw bytes
   v
Sensor Driver
   |
   | converted integer value
   v
IIO Core
   |
   | expose value
   v
sysfs
   |
   | print raw value
   v
Userspace
```

---

