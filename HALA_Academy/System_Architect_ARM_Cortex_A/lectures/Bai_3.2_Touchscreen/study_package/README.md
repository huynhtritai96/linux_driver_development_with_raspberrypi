# Bài 3.2: Touchscreen Driver (Input Subsystem)

## Overview

This self-study package ties the lecture to the closest available LKMPG material without pretending the match is stronger than it is. Confidence for the mapping is **medium**.

## Concepts you must understand

- Hiểu Linux Input Subsystem

## Lecture documents used

- `/home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/PDFs/Bai_3.2_Touchscreen.pdf`
- `/home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/output/Bai_3.2_Touchscreen__raw.md`

## LKMPG source mapping

- Matched topic areas: input subsystem, device-tree related examples
- Mapping note: LKMPG has a virtual input framework but not an I2C touchscreen driver.
- Gap note: Real touchscreen IRQ/I2C/reporting paths are not fully represented; input-device registration is the closest match.

- `vinput.c`
- `vinput.h`
- `devicetree.c`
## Source files in this lecture

- Lecture PDF: `/home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/PDFs/Bai_3.2_Touchscreen.pdf`
- Parsed lecture Markdown: `/home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/output/Bai_3.2_Touchscreen__raw.md`
- Key figure captions:
  Hình 1: Kiến trúc Input Subsystem

## Example mapping

| Example | Lecture topic demonstrated | LKMPG source file(s) | Original/adapted/reconstructed | Concept it helps explain | Why it was selected |
| --- | --- | --- | --- | --- | --- |
| vinput | Input device registration and event plumbing | vinput.c, vinput.h | original | Shows how an input-oriented module exposes a control path and interacts with the input subsystem. | It is the strongest LKMPG-side anchor for Linux input concepts. |
| devicetree | Touch controller DT description and binding | devicetree.c | original | Connects lecture DT bindings to an LKMPG module that actually consumes device-tree properties. | Touchscreen drivers are typically instantiated from DT, so DT consumption still matters even without a real touchscreen sample. |

## How each example relates to the lecture

- `vinput`: demonstrates input device registration and event plumbing and was chosen because it is the strongest lkmpg-side anchor for linux input concepts.
- `devicetree`: demonstrates touch controller dt description and binding and was chosen because touchscreen drivers are typically instantiated from dt, so dt consumption still matters even without a real touchscreen sample.

## Build steps

1. `cd lectures/Bai_3.2_Touchscreen/study_package/examples/vinput`
1. `make clean && make`
1. `cd lectures/Bai_3.2_Touchscreen/study_package/examples/devicetree`
1. `make clean && make`

## Run steps

1. `cd lectures/Bai_3.2_Touchscreen/study_package`
1. `./run.sh`
1. Inspect `outputs/` for actual logs or expected-output files.

## Output

### vinput

Output file: `vinput__expected_output.txt`

```text
Example: vinput
Status: expected_output_only
Reason: execution intentionally skipped because this example is not on the auto-run allowlist for the study package.

Build log:
make -C /lib/modules/6.12.47+rpt-rpi-v8/build M=/home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_3.2_Touchscreen/study_package/examples/vinput modules
make[1]: Entering directory '/usr/src/linux-headers-6.12.47+rpt-rpi-v8'
  CC [M]  /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_3.2_Touchscreen/study_package/examples/vinput/vinput.o
  MODPOST /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_3.2_Touchscreen/study_package/examples/vinput/Module.symvers
  CC [M]  /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_3.2_Touchscreen/study_package/examples/vinput/vinput.mod.o
  CC [M]  /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_3.2_Touchscreen/study_package/examples/vinput/.module-common.o
  LD [M]  /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_3.2_Touchscreen/study_package/examples/vinput/vinput.ko
make[1]: Leaving directory '/usr/src/linux-headers-6.12.47+rpt-rpi-v8'

Expected behavior:
Creates the vinput character-device framework and waits for a provider such as vkbd.
```

### devicetree

Output file: `devicetree__expected_output.txt`

```text
Example: devicetree
Status: expected_output_only
Reason: execution intentionally skipped because this example is not on the auto-run allowlist for the study package.

Build log:
make -C /lib/modules/6.12.47+rpt-rpi-v8/build M=/home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_3.2_Touchscreen/study_package/examples/devicetree modules
make[1]: Entering directory '/usr/src/linux-headers-6.12.47+rpt-rpi-v8'
  CC [M]  /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_3.2_Touchscreen/study_package/examples/devicetree/devicetree.o
  MODPOST /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_3.2_Touchscreen/study_package/examples/devicetree/Module.symvers
  CC [M]  /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_3.2_Touchscreen/study_package/examples/devicetree/devicetree.mod.o
  CC [M]  /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_3.2_Touchscreen/study_package/examples/devicetree/.module-common.o
  LD [M]  /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_3.2_Touchscreen/study_package/examples/devicetree/devicetree.ko
make[1]: Leaving directory '/usr/src/linux-headers-6.12.47+rpt-rpi-v8'

Expected behavior:
Module load succeeds, but probe logs appear only when a matching DT node is present.
```

## Output explanation

- `vinput`: Creates the vinput character-device framework and waits for a provider such as vkbd.
- `devicetree`: Module load succeeds, but probe logs appear only when a matching DT node is present.


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
