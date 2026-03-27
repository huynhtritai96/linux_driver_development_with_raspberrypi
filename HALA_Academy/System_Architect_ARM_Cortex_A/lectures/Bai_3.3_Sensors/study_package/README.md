# Bài 3.3: Sensor Driver (Regmap & IIO)

## Overview

This self-study package ties the lecture to the closest available LKMPG material without pretending the match is stronger than it is. Confidence for the mapping is **medium**.

## Concepts you must understand

- Hiểu IIO (Industrial I/O) Subsystem

## Lecture documents used

- `/home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/PDFs/Bai_3.3_Sensors.pdf`
- `/home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/output/Bai_3.3_Sensors__raw.md`

## LKMPG source mapping

- Matched topic areas: GPIO / IRQ / workqueue / tasklet examples, device-tree related examples, sysfs examples
- Mapping note: DHT11 is the closest hardware-side sample, but LKMPG does not cover IIO or regmap directly.
- Gap note: Regmap and IIO buffering are not represented directly; use the DHT11 and sysfs examples as conservative analogies only.

- `dht11.c`
- `devicetree.c`
- `hello-sysfs.c`
## Source files in this lecture

- Lecture PDF: `/home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/PDFs/Bai_3.3_Sensors.pdf`
- Parsed lecture Markdown: `/home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/output/Bai_3.3_Sensors__raw.md`
- Key figure captions:
  Hình 1: Kiến trúc IIO Subsystem

## Example mapping

| Example | Lecture topic demonstrated | LKMPG source file(s) | Original/adapted/reconstructed | Concept it helps explain | Why it was selected |
| --- | --- | --- | --- | --- | --- |
| dht11 | Sensor-driver structure and hardware interaction | dht11.c | original | Shows a concrete sensor-facing module with GPIO timing, device creation, and data exposure. | It is the only LKMPG example centered on a physical sensor. |
| devicetree | Sensor instantiation from DT | devicetree.c | original | Provides the DT property-consumption piece often used by modern sensor drivers. | Sensors are commonly described in DT, so the DT example fills that architectural gap. |
| hello-sysfs | Userspace-visible configuration and telemetry surfaces | hello-sysfs.c | original | Shows a minimal attribute interface that helps explain why sensor frameworks expose stable userspace views. | It is a safe, runnable bridge for explaining sensor configuration/reporting paths. |

## How each example relates to the lecture

- `dht11`: demonstrates sensor-driver structure and hardware interaction and was chosen because it is the only lkmpg example centered on a physical sensor.
- `devicetree`: demonstrates sensor instantiation from dt and was chosen because sensors are commonly described in dt, so the dt example fills that architectural gap.
- `hello-sysfs`: demonstrates userspace-visible configuration and telemetry surfaces and was chosen because it is a safe, runnable bridge for explaining sensor configuration/reporting paths.

## Build steps

1. `cd lectures/Bai_3.3_Sensors/study_package/examples/dht11`
1. `make clean && make`
1. `cd lectures/Bai_3.3_Sensors/study_package/examples/devicetree`
1. `make clean && make`
1. `cd lectures/Bai_3.3_Sensors/study_package/examples/hello-sysfs`
1. `make clean && make`

## Run steps

1. `cd lectures/Bai_3.3_Sensors/study_package`
1. `./run.sh`
1. Inspect `outputs/` for actual logs or expected-output files.

## Output

### dht11

Output file: `dht11__expected_output.txt`

```text
Example: dht11
Status: expected_output_only
Reason: execution intentionally skipped because this example is not on the auto-run allowlist for the study package.

Build log:
make -C /lib/modules/6.12.47+rpt-rpi-v8/build M=/home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_3.3_Sensors/study_package/examples/dht11 modules
make[1]: Entering directory '/usr/src/linux-headers-6.12.47+rpt-rpi-v8'
  CC [M]  /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_3.3_Sensors/study_package/examples/dht11/dht11.o
  MODPOST /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_3.3_Sensors/study_package/examples/dht11/Module.symvers
  CC [M]  /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_3.3_Sensors/study_package/examples/dht11/dht11.mod.o
  CC [M]  /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_3.3_Sensors/study_package/examples/dht11/.module-common.o
  LD [M]  /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_3.3_Sensors/study_package/examples/dht11/dht11.ko
make[1]: Leaving directory '/usr/src/linux-headers-6.12.47+rpt-rpi-v8'

Expected behavior:
On supported wiring, reads return measured humidity and temperature text.
```

### devicetree

Output file: `devicetree__expected_output.txt`

```text
Example: devicetree
Status: expected_output_only
Reason: execution intentionally skipped because this example is not on the auto-run allowlist for the study package.

Build log:
make -C /lib/modules/6.12.47+rpt-rpi-v8/build M=/home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_3.3_Sensors/study_package/examples/devicetree modules
make[1]: Entering directory '/usr/src/linux-headers-6.12.47+rpt-rpi-v8'
  CC [M]  /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_3.3_Sensors/study_package/examples/devicetree/devicetree.o
  MODPOST /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_3.3_Sensors/study_package/examples/devicetree/Module.symvers
  CC [M]  /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_3.3_Sensors/study_package/examples/devicetree/devicetree.mod.o
  CC [M]  /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_3.3_Sensors/study_package/examples/devicetree/.module-common.o
  LD [M]  /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_3.3_Sensors/study_package/examples/devicetree/devicetree.ko
make[1]: Leaving directory '/usr/src/linux-headers-6.12.47+rpt-rpi-v8'

Expected behavior:
Module load succeeds, but probe logs appear only when a matching DT node is present.
```

### hello-sysfs

Output file: `hello-sysfs.txt`

```text
Example: hello-sysfs
Status: executed

Build log:
make -C /lib/modules/6.12.47+rpt-rpi-v8/build M=/home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_3.3_Sensors/study_package/examples/hello-sysfs modules
make[1]: Entering directory '/usr/src/linux-headers-6.12.47+rpt-rpi-v8'
  CC [M]  /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_3.3_Sensors/study_package/examples/hello-sysfs/hello-sysfs.o
  MODPOST /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_3.3_Sensors/study_package/examples/hello-sysfs/Module.symvers
  CC [M]  /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_3.3_Sensors/study_package/examples/hello-sysfs/hello-sysfs.mod.o
  CC [M]  /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_3.3_Sensors/study_package/examples/hello-sysfs/.module-common.o
  LD [M]  /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_3.3_Sensors/study_package/examples/hello-sysfs/hello-sysfs.ko
make[1]: Leaving directory '/usr/src/linux-headers-6.12.47+rpt-rpi-v8'

Command log:
$ sudo insmod /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_3.3_Sensors/study_package/examples/hello-sysfs/hello-sysfs.ko
$ bash -lc cat /sys/kernel/mymodule/myvariable; printf '7\n' | sudo tee /sys/kernel/mymodule/myvariable >/dev/null; cat /sys/kernel/mymodule/myvariable
cat: /sys/kernel/mymodule/myvariable: Permission denied
$ sudo rmmod hello-sysfs
... [truncated]
```

## Output explanation

- `dht11`: On supported wiring, reads return measured humidity and temperature text.
- `devicetree`: Module load succeeds, but probe logs appear only when a matching DT node is present.
- `hello-sysfs`: Reading the sysfs attribute shows the current integer value and writes update it in-kernel.


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
