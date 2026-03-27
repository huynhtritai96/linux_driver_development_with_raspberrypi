# Extra Notes

## Representative lecture commands

- `sudo apt install -y git build-essential libncurses-dev \`
- `make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- defconfig`
- `make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- menuconfig`
- `make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- -j$(nproc) Image`
- `make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- -j$(nproc) dtbs`

## Figures and diagrams seen in the lecture

- Hình 1: Luồng khởi động Kernel

## Mapping note

The DT-side mapping is real, but LKMPG does not teach kernel image build internals beyond external module compilation.

## Gap note

Kernel image build and early boot are only partially covered; use LKMPG for DT/platform-driver anchoring, not for head.S or full kernel build depth.
