# Bài 2.1: Kernel Build & Device Tree Baseline

## Overview

This self-study package ties the lecture to the closest available LKMPG material without pretending the match is stronger than it is. Confidence for the mapping is **medium**.

## Concepts you must understand

- Hiểu luồng khởi động của Linux Kernel (head.S → start_kernel)

## Lecture documents used

- `/home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/PDFs/Bai_2.1_Kernel_DTB.pdf`
- `/home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/output/Bai_2.1_Kernel_DTB__raw.md`

## LKMPG source mapping

- Matched topic areas: device-tree related examples, device model, kernel modules
- Mapping note: The DT-side mapping is real, but LKMPG does not teach kernel image build internals beyond external module compilation.
- Gap note: Kernel image build and early boot are only partially covered; use LKMPG for DT/platform-driver anchoring, not for head.S or full kernel build depth.

- `devicetree.c`
- `devicemodel.c`
- `hello-1.c`
## Source files in this lecture

- Lecture PDF: `/home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/PDFs/Bai_2.1_Kernel_DTB.pdf`
- Parsed lecture Markdown: `/home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/output/Bai_2.1_Kernel_DTB__raw.md`
- Key figure captions:
  Hình 1: Luồng khởi động Kernel

## Example mapping

| Example | Lecture topic demonstrated | LKMPG source file(s) | Original/adapted/reconstructed | Concept it helps explain | Why it was selected |
| --- | --- | --- | --- | --- | --- |
| devicetree | Device Tree matching and DTB-consumed properties | devicetree.c | original | Shows how a module reads DT properties and probes through of_match_table. | It is the closest LKMPG example to a DTB-backed kernel subsystem hook. |
| devicemodel | Platform-driver registration during kernel/runtime bring-up | devicemodel.c | original | Shows the driver side of platform registration that DT-backed devices ultimately bind to. | It complements the DT example by separating driver-model concepts from DT property parsing. |
| hello-1 | Post-build kernel module validation workflow | hello-1.c | original | Provides the simplest external-module sanity check once kernel headers and build tooling are in place. | It is a conservative runnable anchor for the toolchain-validation part of the lecture. |

## How each example relates to the lecture

- `devicetree`: demonstrates device tree matching and dtb-consumed properties and was chosen because it is the closest lkmpg example to a dtb-backed kernel subsystem hook.
- `devicemodel`: demonstrates platform-driver registration during kernel/runtime bring-up and was chosen because it complements the dt example by separating driver-model concepts from dt property parsing.
- `hello-1`: demonstrates post-build kernel module validation workflow and was chosen because it is a conservative runnable anchor for the toolchain-validation part of the lecture.

## Build steps

1. `cd lectures/Bai_2.1_Kernel_DTB/study_package/examples/devicetree`
1. `make clean && make`
1. `cd lectures/Bai_2.1_Kernel_DTB/study_package/examples/devicemodel`
1. `make clean && make`
1. `cd lectures/Bai_2.1_Kernel_DTB/study_package/examples/hello-1`
1. `make clean && make`

## Run steps

1. `cd lectures/Bai_2.1_Kernel_DTB/study_package`
1. `./run.sh`
1. Inspect `outputs/` for actual logs or expected-output files.

## Output

### devicetree

Output file: `devicetree__expected_output.txt`

```text
Example: devicetree
Status: expected_output_only
Reason: execution intentionally skipped because this example is not on the auto-run allowlist for the study package.

Build log:
make -C /lib/modules/6.12.47+rpt-rpi-v8/build M=/home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_2.1_Kernel_DTB/study_package/examples/devicetree modules
make[1]: Entering directory '/usr/src/linux-headers-6.12.47+rpt-rpi-v8'
  CC [M]  /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_2.1_Kernel_DTB/study_package/examples/devicetree/devicetree.o
  MODPOST /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_2.1_Kernel_DTB/study_package/examples/devicetree/Module.symvers
  CC [M]  /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_2.1_Kernel_DTB/study_package/examples/devicetree/devicetree.mod.o
  CC [M]  /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_2.1_Kernel_DTB/study_package/examples/devicetree/.module-common.o
  LD [M]  /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_2.1_Kernel_DTB/study_package/examples/devicetree/devicetree.ko
make[1]: Leaving directory '/usr/src/linux-headers-6.12.47+rpt-rpi-v8'

Expected behavior:
Module load succeeds, but probe logs appear only when a matching DT node is present.
```

### devicemodel

Output file: `devicemodel__expected_output.txt`

```text
Example: devicemodel
Status: expected_output_only
Reason: execution intentionally skipped because this example is not on the auto-run allowlist for the study package.

Build log:
make -C /lib/modules/6.12.47+rpt-rpi-v8/build M=/home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_2.1_Kernel_DTB/study_package/examples/devicemodel modules
make[1]: Entering directory '/usr/src/linux-headers-6.12.47+rpt-rpi-v8'
  CC [M]  /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_2.1_Kernel_DTB/study_package/examples/devicemodel/devicemodel.o
  MODPOST /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_2.1_Kernel_DTB/study_package/examples/devicemodel/Module.symvers
  CC [M]  /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_2.1_Kernel_DTB/study_package/examples/devicemodel/devicemodel.mod.o
  CC [M]  /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_2.1_Kernel_DTB/study_package/examples/devicemodel/.module-common.o
  LD [M]  /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_2.1_Kernel_DTB/study_package/examples/devicemodel/devicemodel.ko
make[1]: Leaving directory '/usr/src/linux-headers-6.12.47+rpt-rpi-v8'

Expected behavior:
The module registers a platform driver, but probe only runs if a matching platform device exists.
```

### hello-1

Output file: `hello-1.txt`

```text
Example: hello-1
Status: executed

Build log:
make -C /lib/modules/6.12.47+rpt-rpi-v8/build M=/home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_2.1_Kernel_DTB/study_package/examples/hello-1 modules
make[1]: Entering directory '/usr/src/linux-headers-6.12.47+rpt-rpi-v8'
  CC [M]  /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_2.1_Kernel_DTB/study_package/examples/hello-1/hello-1.o
  MODPOST /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_2.1_Kernel_DTB/study_package/examples/hello-1/Module.symvers
  CC [M]  /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_2.1_Kernel_DTB/study_package/examples/hello-1/hello-1.mod.o
  CC [M]  /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_2.1_Kernel_DTB/study_package/examples/hello-1/.module-common.o
  LD [M]  /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_2.1_Kernel_DTB/study_package/examples/hello-1/hello-1.ko
make[1]: Leaving directory '/usr/src/linux-headers-6.12.47+rpt-rpi-v8'

Command log:
$ sudo insmod /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_2.1_Kernel_DTB/study_package/examples/hello-1/hello-1.ko
$ bash -lc sleep 1
$ sudo rmmod hello-1

... [truncated]
```

## Output explanation

- `devicetree`: Module load succeeds, but probe logs appear only when a matching DT node is present.
- `devicemodel`: The module registers a platform driver, but probe only runs if a matching platform device exists.
- `hello-1`: Kernel log prints a hello message on load and a goodbye message on unload.


## Detailed code analysis

The selected examples emphasize init/exit flow, control interfaces, object lifetime, and the narrowest useful kernel API surface. For this lecture, pay attention to how the LKMPG sample either directly models the lecture concept or deliberately stops short of it. That boundary is educational: it tells you where generic kernel-module mechanics end and subsystem-specific engineering begins.

## Kernel / system behavior analysis

- Module init and cleanup are only the visible edges of the lifecycle; the important behavior lives in registration, callback context, and the external interface each example exposes.
- Kernel-space code runs with stricter failure modes than userspace. A short sample can still carry locking, lifetime, or context assumptions that matter in production code.
- Where the package uses expected-output-only files, the blocker is a real environment or coverage limitation, not an omitted execution attempt.

## Common pitfalls

- Treating module-level examples as full subsystem implementations when the lecture is actually about a larger boot, graphics, or media stack.
- Ignoring privilege boundaries: module load/unload, /proc writes, and sysfs writes often require root.
- Assuming successful module load means the example fully exercised the lecture concept; several packages are intentionally partial mappings.
- Forgetting to inspect the `outputs/` directory after `run.sh`; that is where execution blockers and expected-output notes are recorded.
- Missing the manual-review note. This lecture intentionally documents gaps where LKMPG does not provide a direct code sample.

## Suggested next steps

- Re-run `run.sh` after adjusting the target environment or hardware prerequisites.
- Compare the LKMPG example with the lecture commands and diagrams already embedded in the course Markdown.
- Use the analysis files in `analysis/` as a checklist for code reading and debugging practice.
- Add a board- or subsystem-specific reference implementation outside LKMPG if you need a truly direct lab for this lecture.
