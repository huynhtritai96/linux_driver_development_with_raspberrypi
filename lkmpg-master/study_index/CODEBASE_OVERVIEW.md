# LKMPG Codebase Overview

## Top-level structure

- `README.md`: project introduction and document build instructions.
- `lkmpg.tex`: canonical chapter structure and source mapping for topics.
- `Makefile`: builds PDF/HTML guide artifacts, not module examples.
- `examples/`: primary codebase for kernel module samples and helper assets.
- `examples/other/`: userspace helper programs used with selected module labs.
- `.ci/build-n-run.sh`: CI smoke workflow for build/load/unload loops.
- `.ci/non-working`: upstream list of modules excluded from CI runtime loads.
- `study_index/`: generated repository analysis index (this folder).
- `topics/`: generated chapter study units (`NN_slug/study_package`).

## Major topic areas

- Hello-world module lifecycle and Kbuild basics.
- Character devices and device-file interaction.
- `/proc` and `sysfs` interfaces.
- ioctl and mixed kernel/userspace examples.
- Blocking, completions, and synchronization primitives.
- GPIO, interrupts, deferred work (tasklet/workqueue/threaded IRQ).
- Virtual input subsystem, device model, and device tree examples.
- Optimization patterns using static keys.

## Important folders

- `examples/Makefile`: central external-module Kbuild file for all LKMPG modules.
- `examples/chardev.h`: shared ioctl ABI header for kernel/userspace sample pair.
- `examples/vinput.h`: shared virtual-input declarations used by `vinput` and `vkbd`.
- `examples/dt-overlay.dts`: DT overlay sample for devicetree topic.
- `.ci/non-working`: authoritative list of CI-skipped runtime examples.

## Important source files

- `examples/chardev.c`, `examples/chardev2.c`, `examples/ioctl.c`
- `examples/procfs1.c` through `examples/procfs4.c`, `examples/hello-sysfs.c`
- `examples/sleep.c`, `examples/completions.c`
- `examples/example_mutex.c`, `examples/example_spinlock.c`, `examples/example_rwlock.c`, `examples/example_atomic.c`
- `examples/intrpt.c`, `examples/bottomhalf.c`, `examples/bh_threaded.c`
- `examples/vinput.c`, `examples/vkbd.c`
- `examples/devicemodel.c`, `examples/devicetree.c`, `examples/static_key.c`

## Build system overview

- Top-level guide build: `make all` / `make html` from repository root.
- Example build: `make -C /lib/modules/$(uname -r)/build M=<lkmpg-root>/examples <module>.ko`.
- Whole example tree build: `make -C examples` (fails here due `syscall-steal` on ARM64).
- CI strategy: build all modules then load/unload only modules not listed in `.ci/non-working`.

## Kernel/module assumptions

- Target kernel family: modern 5.x/6.x, with compatibility guards in many samples.
- Runtime module operations (`insmod`, `rmmod`) require root privileges.
- Many examples assume running-kernel headers exist at `/lib/modules/$(uname -r)/build`.
- Several hardware-facing samples assume board-specific GPIO numbering and wiring.

## Environment requirements

- Matching kernel headers/toolchain for the running kernel.
- Root capability (`sudo`) for module insertion/removal and privileged node access.
- Optional userspace toolchain (`gcc`) for helper binaries in `examples/other/`.
- Hardware dependencies for GPIO/IRQ/sensor examples.
- DT overlay tools/runtime support (`dtc`, overlay loader) for full device-tree overlay flow.

## Gaps or ambiguous areas

- Repository is chapter-organized in `lkmpg.tex`, but source files are mostly flat in `examples/`.
- No native per-chapter source directories; chapter-topic mapping is inferred from TeX references.
- `devicemodel` and `devicetree` compile/load cleanly but probe behavior depends on external platform/DT context.
- `vkbd` requires explicit out-of-tree symbol linking (`KBUILD_EXTRA_SYMBOLS`) with `vinput`.
