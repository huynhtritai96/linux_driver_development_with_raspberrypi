# Extra Notes

## Representative lecture commands

- `cat /sys/bus/iio/devices/iio:device0/in_accel_x_raw`
- `cat /sys/bus/iio/devices/iio:device0/in_accel_y_raw`
- `cat /sys/bus/iio/devices/iio:device0/in_accel_z_raw`
- `cat /sys/bus/iio/devices/iio:device0/in_accel_scale`

## Figures and diagrams seen in the lecture

- Hình 1: Kiến trúc IIO Subsystem

## Mapping note

DHT11 is the closest hardware-side sample, but LKMPG does not cover IIO or regmap directly.

## Gap note

Regmap and IIO buffering are not represented directly; use the DHT11 and sysfs examples as conservative analogies only.
