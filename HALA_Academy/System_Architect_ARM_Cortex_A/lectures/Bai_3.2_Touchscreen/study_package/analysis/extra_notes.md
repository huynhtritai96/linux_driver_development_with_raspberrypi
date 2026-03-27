# Extra Notes

## Representative lecture commands

- `cat /proc/bus/input/devices`
- `cat /sys/bus/i2c/devices/1-0038/of_node/compatible`

## Figures and diagrams seen in the lecture

- Hình 1: Kiến trúc Input Subsystem

## Mapping note

LKMPG has a virtual input framework but not an I2C touchscreen driver.

## Gap note

Real touchscreen IRQ/I2C/reporting paths are not fully represented; input-device registration is the closest match.
