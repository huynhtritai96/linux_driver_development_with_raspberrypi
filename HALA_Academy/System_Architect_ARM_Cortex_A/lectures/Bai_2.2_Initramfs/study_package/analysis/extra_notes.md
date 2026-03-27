# Extra Notes

## Representative lecture commands

- `mount -o remount,rw /`
- `mount -t proc proc /proc`
- `mount -t sysfs sysfs /sys`
- `mount -t devtmpfs devtmpfs /dev`
- `echo "Welcome to Minimal Initramfs"`
- `mount -t ext4 "$ROOT_DEV" /mnt/root`
- `make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- defconfig`
- `make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- menuconfig`
- `make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- -j$(nproc)`
- `make CONFIG_PREFIX=../initramfs install`
- `sudo mknod -m 600 dev/console c 5 1`
- `sudo mknod -m 666 dev/null c 1 3`

## Figures and diagrams seen in the lecture

- Hình 1: Luồng boot với Initramfs

## Mapping note

LKMPG assumes a running Linux userspace and does not include initramfs assembly flows.

## Gap note

No close LKMPG example exists for initramfs init scripts, BusyBox rootfs assembly, or switch_root transitions.
