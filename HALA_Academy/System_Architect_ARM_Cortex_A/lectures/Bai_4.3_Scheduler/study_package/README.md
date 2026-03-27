# Bài 4.3: Advanced Scheduler & Isolation

## Overview

This self-study package ties the lecture to the closest available LKMPG material without pretending the match is stronger than it is. Confidence for the mapping is **high**.

## Concepts you must understand

- Phân tích Latency sâu với `perf sched`
- Quản lý tài nguyên hệ thống với Cgroups v2
- System Tuning nâng cao với sysctl

## Lecture documents used

- `/home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/PDFs/Bai_4.3_Scheduler.pdf`
- `/home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/output/Bai_4.3_Scheduler__raw.md`

## LKMPG source mapping

- Matched topic areas: GPIO / IRQ / workqueue / tasklet examples, synchronization examples
- Mapping note: LKMPG has clean runnable examples for workqueues, completions, and deferred work.
- Gap note: cgroups v2 and sysctl tuning remain outside LKMPG and are documented as lecture-only material.

- `sched.c`
- `completions.c`
- `example_tasklet.c`
## Source files in this lecture

- Lecture PDF: `/home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/PDFs/Bai_4.3_Scheduler.pdf`
- Parsed lecture Markdown: `/home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/output/Bai_4.3_Scheduler__raw.md`
- Key figure captions:
  Hình 1: Scheduling Context Switch & Latency Analysis
  Hình 2: Các Kernel Preemption Models

## Example mapping

| Example | Lecture topic demonstrated | LKMPG source file(s) | Original/adapted/reconstructed | Concept it helps explain | Why it was selected |
| --- | --- | --- | --- | --- | --- |
| sched | Workqueue scheduling and deferred execution | sched.c | original | Provides a concrete queued-work path for explaining asynchronous scheduler-visible tasks. | It is the closest direct mapping for scheduler-deferred work in LKMPG. |
| completions | Synchronization between scheduled kernel threads | completions.c | original | Shows ordering constraints across kthreads, which helps explain latency-sensitive coordination. | It complements workqueues with explicit wake-up sequencing. |
| example_tasklet | Bottom-half style deferred execution | example_tasklet.c | original | Illustrates old-style deferred execution, useful for contrasting latency trade-offs. | It rounds out the lecture’s discussion of scheduling-sensitive deferred work mechanisms. |

## How each example relates to the lecture

- `sched`: demonstrates workqueue scheduling and deferred execution and was chosen because it is the closest direct mapping for scheduler-deferred work in lkmpg.
- `completions`: demonstrates synchronization between scheduled kernel threads and was chosen because it complements workqueues with explicit wake-up sequencing.
- `example_tasklet`: demonstrates bottom-half style deferred execution and was chosen because it rounds out the lecture’s discussion of scheduling-sensitive deferred work mechanisms.

## Build steps

1. `cd lectures/Bai_4.3_Scheduler/study_package/examples/sched`
1. `make clean && make`
1. `cd lectures/Bai_4.3_Scheduler/study_package/examples/completions`
1. `make clean && make`
1. `cd lectures/Bai_4.3_Scheduler/study_package/examples/example_tasklet`
1. `make clean && make`

## Run steps

1. `cd lectures/Bai_4.3_Scheduler/study_package`
1. `./run.sh`
1. Inspect `outputs/` for actual logs or expected-output files.

## Output

### sched

Output file: `sched.txt`

```text
Example: sched
Status: executed

Build log:
make -C /lib/modules/6.12.47+rpt-rpi-v8/build M=/home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_4.3_Scheduler/study_package/examples/sched modules
make[1]: Entering directory '/usr/src/linux-headers-6.12.47+rpt-rpi-v8'
  CC [M]  /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_4.3_Scheduler/study_package/examples/sched/sched.o
  MODPOST /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_4.3_Scheduler/study_package/examples/sched/Module.symvers
  CC [M]  /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_4.3_Scheduler/study_package/examples/sched/sched.mod.o
  CC [M]  /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_4.3_Scheduler/study_package/examples/sched/.module-common.o
  LD [M]  /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_4.3_Scheduler/study_package/examples/sched/sched.ko
make[1]: Leaving directory '/usr/src/linux-headers-6.12.47+rpt-rpi-v8'

Command log:
$ sudo insmod /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_4.3_Scheduler/study_package/examples/sched/sched.ko
$ bash -lc sleep 1
$ sudo rmmod sched

... [truncated]
```

### completions

Output file: `completions.txt`

```text
Example: completions
Status: executed

Build log:
make -C /lib/modules/6.12.47+rpt-rpi-v8/build M=/home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_4.3_Scheduler/study_package/examples/completions modules
make[1]: Entering directory '/usr/src/linux-headers-6.12.47+rpt-rpi-v8'
  CC [M]  /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_4.3_Scheduler/study_package/examples/completions/completions.o
  MODPOST /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_4.3_Scheduler/study_package/examples/completions/Module.symvers
  CC [M]  /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_4.3_Scheduler/study_package/examples/completions/completions.mod.o
  CC [M]  /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_4.3_Scheduler/study_package/examples/completions/.module-common.o
  LD [M]  /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_4.3_Scheduler/study_package/examples/completions/completions.ko
make[1]: Leaving directory '/usr/src/linux-headers-6.12.47+rpt-rpi-v8'

Command log:
$ sudo insmod /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_4.3_Scheduler/study_package/examples/completions/completions.ko
$ bash -lc sleep 1
$ sudo rmmod completions

... [truncated]
```

### example_tasklet

Output file: `example_tasklet.txt`

```text
Example: example_tasklet
Status: executed

Build log:
make -C /lib/modules/6.12.47+rpt-rpi-v8/build M=/home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_4.3_Scheduler/study_package/examples/example_tasklet modules
make[1]: Entering directory '/usr/src/linux-headers-6.12.47+rpt-rpi-v8'
  CC [M]  /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_4.3_Scheduler/study_package/examples/example_tasklet/example_tasklet.o
  MODPOST /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_4.3_Scheduler/study_package/examples/example_tasklet/Module.symvers
  CC [M]  /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_4.3_Scheduler/study_package/examples/example_tasklet/example_tasklet.mod.o
  CC [M]  /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_4.3_Scheduler/study_package/examples/example_tasklet/.module-common.o
  LD [M]  /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_4.3_Scheduler/study_package/examples/example_tasklet/example_tasklet.ko
make[1]: Leaving directory '/usr/src/linux-headers-6.12.47+rpt-rpi-v8'

Command log:
$ sudo insmod /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_4.3_Scheduler/study_package/examples/example_tasklet/example_tasklet.ko
$ bash -lc sleep 6
$ sudo rmmod example_tasklet

... [truncated]
```

## Output explanation

- `sched`: Kernel log shows workqueue allocation, queued work execution, and cleanup.
- `completions`: Kernel log shows the crank/flywheel sequence in completion order.
- `example_tasklet`: Tasklet output appears asynchronously after module init scheduling.


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
