# Build Notes

## Common build patterns

- Primary module build pattern:
  - `make -C /lib/modules/$(uname -r)/build M=<lkmpg-root>/examples <module>.ko`
- Full tree build pattern:
  - `make -C <lkmpg-root>/examples`
- Multi-file module pattern:
  - `startstop-objs := start.o stop.o`
- Userspace helper build pattern:
  - `gcc -O2 -Wall -Wextra <helper>.c -o <helper>`

## Makefile conventions

- `obj-m += <name>.o` registers module targets for external Kbuild.
- `KDIR ?= /lib/modules/$(uname -r)/build` ties default build to running kernel headers.
- `M=$(PWD)` exposes the example directory as an external module source tree.

## Kernel-version concerns

- Compatibility guards used in examples for API drift:
  - `proc_ops` vs `file_operations`
  - `class_create` signature changes
  - kernel-version-specific helper names (`kthread_complete_and_exit`, etc.)
- `syscall-steal` is highly kernel/arch sensitive and fails on this ARM64 host.
- `vkbd` out-of-tree build needs symbol linkage to `vinput` via `KBUILD_EXTRA_SYMBOLS`.

## Module loading/unloading notes

- Most samples rely on `insmod`/`rmmod` rather than module alias resolution via `modprobe`.
- Character-device, procfs, and sysfs modules require post-load user interaction to exercise core callbacks.
- Hardware examples may load-fail even after successful build if GPIO/IRQ/sensor resources are unavailable.

## Troubleshooting notes

- Verify headers: `/lib/modules/$(uname -r)/build` must exist and match running kernel.
- On load failure: inspect `dmesg` immediately after `insmod`.
- For missing symbols between out-of-tree modules (e.g., `vkbd` <-> `vinput`), rebuild with `KBUILD_EXTRA_SYMBOLS`.
- For GPIO failures (`-517`, `-16`), confirm line availability, pin mapping, and hardware wiring.
- For sysfs permission errors, use `sudo cat`/`sudo tee` for privileged paths.
