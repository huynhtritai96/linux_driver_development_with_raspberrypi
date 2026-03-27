# Bài 7.3: Real-Time Design Patterns

## Overview

This self-study package ties the lecture to the closest available LKMPG material without pretending the match is stronger than it is. Confidence for the mapping is **high**.

## Concepts you must understand

- Áp dụng các mẫu thiết kế (Design Patterns) an toàn cho Real-time.
- Giải quyết vấn đề Priority Inversion với PI Mutex.
- Thiết kế cơ chế giao tiếp Lock-free giữa RT thread và Non-RT thread.

## Lecture documents used

- `/home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/PDFs/Bai_7.3_RT_Workload.pdf`
- `/home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/output/Bai_7.3_RT_Workload__raw.md`

## LKMPG source mapping

- Matched topic areas: synchronization examples
- Mapping note: LKMPG does not provide RT user-space threads, but it does provide the key kernel-side synchronization primitives.
- Gap note: Full RT workload shaping and lock-free queues are only partially represented; the package calls that out explicitly.

- `example_mutex.c`
- `completions.c`
- `example_atomic.c`
## Source files in this lecture

- Lecture PDF: `/home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/PDFs/Bai_7.3_RT_Workload.pdf`
- Parsed lecture Markdown: `/home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/output/Bai_7.3_RT_Workload__raw.md`
- Key figure captions:
  Hình 1: Cơ chế Priority Inheritance giải quyết vấn đề

## Example mapping

| Example | Lecture topic demonstrated | LKMPG source file(s) | Original/adapted/reconstructed | Concept it helps explain | Why it was selected |
| --- | --- | --- | --- | --- | --- |
| example_mutex | Priority inversion and blocking critical sections | example_mutex.c | original | Gives a clear mutex-based starting point for explaining why priority inheritance matters. | It is the smallest direct example of a sleepable lock relevant to RT workloads. |
| completions | Deterministic phase ordering between actors | completions.c | original | Shows an explicit hand-off primitive that fits RT design-pattern discussions. | Completion-based ordering is a good contrast to busy waiting and ad hoc polling. |
| example_atomic | Lock-free or low-lock state transitions | example_atomic.c | original | Illustrates when atomic operations can replace heavier locking for very small shared-state updates. | It gives a concrete low-lock example for the lecture’s lock-free design discussion. |

## How each example relates to the lecture

- `example_mutex`: demonstrates priority inversion and blocking critical sections and was chosen because it is the smallest direct example of a sleepable lock relevant to rt workloads.
- `completions`: demonstrates deterministic phase ordering between actors and was chosen because completion-based ordering is a good contrast to busy waiting and ad hoc polling.
- `example_atomic`: demonstrates lock-free or low-lock state transitions and was chosen because it gives a concrete low-lock example for the lecture’s lock-free design discussion.

## Build steps

1. `cd lectures/Bai_7.3_RT_Workload/study_package/examples/example_mutex`
1. `make clean && make`
1. `cd lectures/Bai_7.3_RT_Workload/study_package/examples/completions`
1. `make clean && make`
1. `cd lectures/Bai_7.3_RT_Workload/study_package/examples/example_atomic`
1. `make clean && make`

## Run steps

1. `cd lectures/Bai_7.3_RT_Workload/study_package`
1. `./run.sh`
1. Inspect `outputs/` for actual logs or expected-output files.

## Output

### example_mutex

Output file: `example_mutex.txt`

```text
Example: example_mutex
Status: executed

Build log:
make -C /lib/modules/6.12.47+rpt-rpi-v8/build M=/home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_7.3_RT_Workload/study_package/examples/example_mutex modules
make[1]: Entering directory '/usr/src/linux-headers-6.12.47+rpt-rpi-v8'
  CC [M]  /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_7.3_RT_Workload/study_package/examples/example_mutex/example_mutex.o
  MODPOST /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_7.3_RT_Workload/study_package/examples/example_mutex/Module.symvers
  CC [M]  /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_7.3_RT_Workload/study_package/examples/example_mutex/example_mutex.mod.o
  CC [M]  /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_7.3_RT_Workload/study_package/examples/example_mutex/.module-common.o
  LD [M]  /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_7.3_RT_Workload/study_package/examples/example_mutex/example_mutex.ko
make[1]: Leaving directory '/usr/src/linux-headers-6.12.47+rpt-rpi-v8'

Command log:
$ sudo insmod /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_7.3_RT_Workload/study_package/examples/example_mutex/example_mutex.ko
$ bash -lc sleep 1
$ sudo rmmod example_mutex

... [truncated]
```

### completions

Output file: `completions.txt`

```text
Example: completions
Status: executed

Build log:
make -C /lib/modules/6.12.47+rpt-rpi-v8/build M=/home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_7.3_RT_Workload/study_package/examples/completions modules
make[1]: Entering directory '/usr/src/linux-headers-6.12.47+rpt-rpi-v8'
  CC [M]  /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_7.3_RT_Workload/study_package/examples/completions/completions.o
  MODPOST /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_7.3_RT_Workload/study_package/examples/completions/Module.symvers
  CC [M]  /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_7.3_RT_Workload/study_package/examples/completions/completions.mod.o
  CC [M]  /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_7.3_RT_Workload/study_package/examples/completions/.module-common.o
  LD [M]  /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_7.3_RT_Workload/study_package/examples/completions/completions.ko
make[1]: Leaving directory '/usr/src/linux-headers-6.12.47+rpt-rpi-v8'

Command log:
$ sudo insmod /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_7.3_RT_Workload/study_package/examples/completions/completions.ko
$ bash -lc sleep 1
$ sudo rmmod completions

... [truncated]
```

### example_atomic

Output file: `example_atomic.txt`

```text
Example: example_atomic
Status: executed

Build log:
make -C /lib/modules/6.12.47+rpt-rpi-v8/build M=/home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_7.3_RT_Workload/study_package/examples/example_atomic modules
make[1]: Entering directory '/usr/src/linux-headers-6.12.47+rpt-rpi-v8'
  CC [M]  /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_7.3_RT_Workload/study_package/examples/example_atomic/example_atomic.o
  MODPOST /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_7.3_RT_Workload/study_package/examples/example_atomic/Module.symvers
  CC [M]  /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_7.3_RT_Workload/study_package/examples/example_atomic/example_atomic.mod.o
  CC [M]  /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_7.3_RT_Workload/study_package/examples/example_atomic/.module-common.o
  LD [M]  /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_7.3_RT_Workload/study_package/examples/example_atomic/example_atomic.ko
make[1]: Leaving directory '/usr/src/linux-headers-6.12.47+rpt-rpi-v8'

Command log:
$ sudo insmod /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_7.3_RT_Workload/study_package/examples/example_atomic/example_atomic.ko
$ bash -lc sleep 1
$ sudo rmmod example_atomic

... [truncated]
```

## Output explanation

- `example_mutex`: Kernel log shows successful mutex acquisition and release.
- `completions`: Kernel log shows the crank/flywheel sequence in completion order.
- `example_atomic`: Kernel log prints atomic counter updates and bit patterns.


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
