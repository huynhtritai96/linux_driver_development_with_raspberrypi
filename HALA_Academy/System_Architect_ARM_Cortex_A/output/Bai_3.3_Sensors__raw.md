# Bài 3.3: Sensor Driver (Regmap & IIO)

## Page 1

# Bài 3.3: Sensor Driver (Regmap & IIO)

# Biên soạn: Phạm Văn Vũ

## Page 2

### Mục tiêu Bài học

Sau buổi học này, học viên sẽ có khả năng:

- Hiểu IIO (Industrial I/O) Subsystem

- Sử dụng Regmap API cho I2C/SPI registers

- Viết IIO sensor driver

## Page 3

### Phần 1: IIO Subsystem

*Hình 1: Kiến trúc IIO Subsystem*
<!-- mermaid-insert:start:bai_3_3_hinh_1 -->
```mermaid
flowchart TD
    subgraph userspace["Userspace"]
        app["Application"]
        libiio["libiio"]
        sysfs["/sys/bus/iio/devices/iio:device0/"]
    end

    subgraph core["Kernel - IIO Core"]
        idev["iio_device"]
        ibuf["iio_buffer"]
        ichan["iio_channel"]
        itrig["iio_trigger"]
    end

    subgraph driver["Kernel - Driver"]
        sensor["IIO Sensor Driver"]
        regmap["Regmap API"]
    end

    subgraph hw["Hardware"]
        dev["I2C/SPI Sensor"]
    end

    app -->|cat in_temp_raw| sysfs
    libiio -->|/dev/iio:device0| idev
    sysfs -->|Export| idev
    idev -->|Optional| ibuf
    idev -->|Contains| ichan
    itrig -->|Trigger| ibuf
    sensor -->|read_raw()| ichan
    sensor --> regmap
    regmap -->|I2C/SPI| dev

    sysfs_note["/sys/bus/iio/devices/iio:device0/<br/>in_temp_raw<br/>in_temp_scale<br/>name"]
    chan_note["struct iio_chan_spec {<br/>type = IIO_TEMP,<br/>info_mask_separate = BIT(IIO_CHAN_INFO_RAW),<br/>}"]
    sysfs -.-> sysfs_note
    ichan -.-> chan_note
```
<!-- mermaid-insert:end:bai_3_3_hinh_1 -->

### 1.1 IIO là gì?

- IIO: Subsystem cho sensors (ADC, DAC, accelerometer, gyro, light...)
- Standard sysfs interface

## Page 4

- Buffer support (continuous sampling)
- Trigger mechanism

### 1.2 IIO Channel Types

Type                    Macro                           Mô tả

Voltage                 IIO_VOLTAGE                     ADC input

Current                 IIO_CURRENT                     Current sensor

Accel                   IIO_ACCEL                       Accelerometer

Gyro                    IIO_ANGL_VEL                    Gyroscope

Light                   IIO_LIGHT                       Ambient light

Temp                    IIO_TEMP                        Temperature

### Phần 2: Regmap API

### 2.1 Tại sao dùng Regmap?

- Abstract hóa I2C/SPI register access
- Caching, locking built-in
- Debug via debugfs

### 2.2 Regmap Config

```text
    static const struct regmap_config my_sensor_regmap_config = {
        .reg_bits = 8,
        .val_bits = 8,
        .max_register = 0xFF,
        .cache_type = REGCACHE_RBTREE,
    };
```

// In probe

## Page 5

```text
    struct regmap *regmap;
    regmap = devm_regmap_init_i2c(client, &my_sensor_regmap_config);
```

### 2.3 Regmap Operations

```text
    // Read single register
    int val;
    regmap_read(regmap, REG_STATUS, &val);
```

```text
    // Write single register
    regmap_write(regmap, REG_CTRL, 0x01);
```

```text
    // Read-modify-write
    regmap_update_bits(regmap, REG_CTRL, MASK_ENABLE, MASK_ENABLE);
```

```text
    // Bulk read
    u8 buf[6];
    regmap_bulk_read(regmap, REG_DATA, buf, 6);
```

## Page 6

### Phần 3: IIO Driver Structure

### 3.1 IIO Channel Definition

```text
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

```text
    static int my_sensor_read_raw(struct iio_dev *indio_dev,
                                   struct iio_chan_spec const *chan,
                                   int *val, int *val2, long mask)
    {
        struct my_sensor_data *data = iio_priv(indio_dev);
        __le16 raw;
```

```text
        switch (mask) {
        case IIO_CHAN_INFO_RAW:
            regmap_bulk_read(data->regmap,
                             REG_ACCEL_X + chan->scan_index * 2,
                             &raw, sizeof(raw));
            *val = sign_extend32(le16_to_cpu(raw), 15);
            return IIO_VAL_INT;
```

```text
        case IIO_CHAN_INFO_SCALE:
            *val = 0;
            *val2 = 9810; // 0.009810 m/s^2 per LSB
            return IIO_VAL_INT_PLUS_MICRO;
```

## Page 7

```text
        default:
               return -EINVAL;
        }
    }
```

### 3.3 Probe Function

```text
    static int my_sensor_probe(struct i2c_client *client)
    {
        struct iio_dev *indio_dev;
        struct my_sensor_data *data;
```

```text
        indio_dev = devm_iio_device_alloc(&client->dev, sizeof(*data));
        data = iio_priv(indio_dev);
```

data->regmap = devm_regmap_init_i2c(client, &my_sensor_regmap_config);

```text
        indio_dev->name = "my-sensor";
        indio_dev->modes = INDIO_DIRECT_MODE;
        indio_dev->channels = my_sensor_channels;
        indio_dev->num_channels = ARRAY_SIZE(my_sensor_channels);
        indio_dev->info = &my_sensor_info;
```

```text
        return devm_iio_device_register(&client->dev, indio_dev);
    }
```

### Phần 4: Sysfs Interface

```text
    # List IIO devices
    ls /sys/bus/iio/devices/
```

```text
    # Read raw values
    cat /sys/bus/iio/devices/iio:device0/in_accel_x_raw
    cat /sys/bus/iio/devices/iio:device0/in_accel_y_raw
    cat /sys/bus/iio/devices/iio:device0/in_accel_z_raw
```

```text
    # Read scale
    cat /sys/bus/iio/devices/iio:device0/in_accel_scale
```

# Calculate real value: raw * scale

## Page 8

### Phần 5: Device Tree

```text
    &i2c1 {
        status = "okay";
```

```text
         accelerometer@1d {
             compatible = "vendor,my-sensor";
               reg = <0x1d>;
               interrupt-parent = <&pio>;
               interrupts = <7 5 IRQ_TYPE_EDGE_RISING>;    // PH5
         };
    };
```

### Phần 6: Câu hỏi Ôn tập

1. IIO subsystem dùng cho loại device nào?

2. Regmap có những ưu điểm gì?

3. IIO_CHAN_INFO_RAW và IIO_CHAN_INFO_SCALE khác nhau?

4. Làm sao đọc sensor qua sysfs?

5. scan_type trong iio_chan_spec có ý nghĩa gì?

Tài liệu Tham khảo

- IIO Documentation
- Regmap API
- IIO Tools (libiio)

Yêu cầu Bài tập

- Hiểu IIO channel structure

## Page 9

- Sử dụng Regmap cho I2C access
- Đọc sensor qua sysfs interface

HALA Academy | Biên soạn: Phạm Văn Vũ
