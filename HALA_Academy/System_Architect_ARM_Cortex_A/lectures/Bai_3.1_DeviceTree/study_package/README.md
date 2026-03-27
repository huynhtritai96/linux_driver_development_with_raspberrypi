# Bài 3.1: Device Tree Chuyên Sâu

## Overview

This self-study package ties the lecture to the closest available LKMPG material without pretending the match is stronger than it is. Confidence for the mapping is **high**.

## Concepts you must understand

- Hiểu sâu cấu trúc và cú pháp Device Tree

## Lecture documents used

- `/home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/PDFs/Bai_3.1_DeviceTree.pdf`
- `/home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/output/Bai_3.1_DeviceTree__raw.md`

## LKMPG source mapping

- Matched topic areas: device-tree related examples, device model
- Mapping note: This is one of the cleanest lecture-to-LKMPG mappings in the course.
- Gap note: Overlay deployment and board-specific bindings still require manual adaptation to the target hardware tree.

- `devicetree.c`
- `devicemodel.c`
## Source files in this lecture

- Lecture PDF: `/home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/PDFs/Bai_3.1_DeviceTree.pdf`
- Parsed lecture Markdown: `/home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/output/Bai_3.1_DeviceTree__raw.md`
- Key figure captions:
  Hình 1: Cấu trúc Device Tree

## Example mapping

| Example | Lecture topic demonstrated | LKMPG source file(s) | Original/adapted/reconstructed | Concept it helps explain | Why it was selected |
| --- | --- | --- | --- | --- | --- |
| devicetree | of_match_table, property reads, and DT-backed probe flow | devicetree.c | original | Demonstrates property parsing, default handling, and platform_set_drvdata usage. | It directly exercises the DT concepts the lecture explains. |
| devicemodel | Platform-driver lifecycle around DT-discovered devices | devicemodel.c | original | Helps separate the generic platform-driver lifecycle from DT-specific match logic. | It clarifies which part of the driver stack is DT-specific and which part is generic. |

## How each example relates to the lecture

- `devicetree`: demonstrates of_match_table, property reads, and dt-backed probe flow and was chosen because it directly exercises the dt concepts the lecture explains.
- `devicemodel`: demonstrates platform-driver lifecycle around dt-discovered devices and was chosen because it clarifies which part of the driver stack is dt-specific and which part is generic.

## Build steps

1. `cd lectures/Bai_3.1_DeviceTree/study_package/examples/devicetree`
1. `make clean && make`
1. `cd lectures/Bai_3.1_DeviceTree/study_package/examples/devicemodel`
1. `make clean && make`

## Run steps

1. `cd lectures/Bai_3.1_DeviceTree/study_package`
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
make -C /lib/modules/6.12.47+rpt-rpi-v8/build M=/home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_3.1_DeviceTree/study_package/examples/devicetree modules
make[1]: Entering directory '/usr/src/linux-headers-6.12.47+rpt-rpi-v8'
  CC [M]  /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_3.1_DeviceTree/study_package/examples/devicetree/devicetree.o
  MODPOST /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_3.1_DeviceTree/study_package/examples/devicetree/Module.symvers
  CC [M]  /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_3.1_DeviceTree/study_package/examples/devicetree/devicetree.mod.o
  CC [M]  /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_3.1_DeviceTree/study_package/examples/devicetree/.module-common.o
  LD [M]  /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_3.1_DeviceTree/study_package/examples/devicetree/devicetree.ko
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
make -C /lib/modules/6.12.47+rpt-rpi-v8/build M=/home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_3.1_DeviceTree/study_package/examples/devicemodel modules
make[1]: Entering directory '/usr/src/linux-headers-6.12.47+rpt-rpi-v8'
  CC [M]  /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_3.1_DeviceTree/study_package/examples/devicemodel/devicemodel.o
  MODPOST /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_3.1_DeviceTree/study_package/examples/devicemodel/Module.symvers
  CC [M]  /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_3.1_DeviceTree/study_package/examples/devicemodel/devicemodel.mod.o
  CC [M]  /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_3.1_DeviceTree/study_package/examples/devicemodel/.module-common.o
  LD [M]  /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_3.1_DeviceTree/study_package/examples/devicemodel/devicemodel.ko
make[1]: Leaving directory '/usr/src/linux-headers-6.12.47+rpt-rpi-v8'

Expected behavior:
The module registers a platform driver, but probe only runs if a matching platform device exists.
```

## Output explanation

- `devicetree`: Module load succeeds, but probe logs appear only when a matching DT node is present.
- `devicemodel`: The module registers a platform driver, but probe only runs if a matching platform device exists.


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

## Suggested next steps

- Re-run `run.sh` after adjusting the target environment or hardware prerequisites.
- Compare the LKMPG example with the lecture commands and diagrams already embedded in the course Markdown.
- Use the analysis files in `analysis/` as a checklist for code reading and debugging practice.
