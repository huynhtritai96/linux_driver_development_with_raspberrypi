# Extra Notes

## Representative lecture commands

- `make CROSS_COMPILE=aarch64-linux-gnu- orangepi_zero3_defconfig`
- `make CROSS_COMPILE=aarch64-linux-gnu- -j$(nproc)`
- `sudo dd if=u-boot-sunxi-with-spl.bin of=/dev/sdX bs=1k seek=8 conv=notrunc`

## Figures and diagrams seen in the lecture

- Hình 1: Kiến trúc bộ nhớ U-Boot

## Mapping note

LKMPG does not include bootloader examples or U-Boot memory-layout code.

## Gap note

No close LKMPG example exists for SPL, U-Boot proper, or bootloader DRAM relocation.
