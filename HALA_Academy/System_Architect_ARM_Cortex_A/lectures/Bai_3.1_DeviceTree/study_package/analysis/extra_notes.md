# Extra Notes

## Representative lecture commands

- `dtc -@ -I dts -O dtb -o my_overlay.dtbo my_overlay.dts`
- `cat my_overlay.dtbo > /sys/kernel/config/device-tree/overlays/my_overlay/dtbo`
- `cat /proc/device-tree/model`
- `dtc -I fs /proc/device-tree > runtime.dts`

## Figures and diagrams seen in the lecture

- Hình 1: Cấu trúc Device Tree

## Mapping note

This is one of the cleanest lecture-to-LKMPG mappings in the course.

## Gap note

Overlay deployment and board-specific bindings still require manual adaptation to the target hardware tree.
