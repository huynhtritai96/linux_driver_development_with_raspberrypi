# Bài 7.1: Advanced Real-Time Linux Analysis

## Overview

This self-study package ties the lecture to the closest available LKMPG material without pretending the match is stronger than it is. Confidence for the mapping is **high**.

## Concepts you must understand

- Phân tích chi tiết PREEMPT_RT patch biến Linux thành Hard Real-Time OS.
- Sử dụng Ftrace (irqsoff, wakeup_rt) để debug latency ở cấp độ microsecond.
- Phát hiện các nguồn trễ phần cứng (SMI, Cache miss) với `hwlatdetect`.

## Lecture documents used

- `/home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/PDFs/Bai_7.1_PreemptRT.pdf`
- `/home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/output/Bai_7.1_PreemptRT__raw.md`

## LKMPG source mapping

- Matched topic areas: synchronization examples
- Mapping note: LKMPG cannot patch the kernel into PREEMPT_RT, but it does provide the locking primitives the lecture analyzes.
- Gap note: The examples explain lock behavior, not the full PREEMPT_RT patchset or tracing tooling.

- `example_mutex.c`
- `example_spinlock.c`
- `example_rwlock.c`
## Source files in this lecture

- Lecture PDF: `/home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/PDFs/Bai_7.1_PreemptRT.pdf`
- Parsed lecture Markdown: `/home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/output/Bai_7.1_PreemptRT__raw.md`
- Key figure captions:
  Hình 1: Sự khác biệt trong xử lý ngắt và critical section

## Example mapping

| Example | Lecture topic demonstrated | LKMPG source file(s) | Original/adapted/reconstructed | Concept it helps explain | Why it was selected |
| --- | --- | --- | --- | --- | --- |
| example_mutex | Sleeping locks and priority-aware critical sections | example_mutex.c | original | Provides a small mutex example for discussing why mutexes are favored over raw spinning in many RT paths. | Mutex behavior is central to the lecture’s PREEMPT_RT comparison story. |
| example_spinlock | Spin-based critical sections and IRQ masking | example_spinlock.c | original | Shows the exact lock family whose semantics change most under RT constraints. | It gives a concrete anchor for explaining latency risks from spinning with interrupts disabled. |
| example_rwlock | Reader/writer lock trade-offs under latency pressure | example_rwlock.c | original | Helps explain how read-mostly locking still carries IRQ-state and contention implications. | It broadens the locking comparison beyond simple mutex-vs-spinlock narratives. |

## How each example relates to the lecture

- `example_mutex`: demonstrates sleeping locks and priority-aware critical sections and was chosen because mutex behavior is central to the lecture’s preempt_rt comparison story.
- `example_spinlock`: demonstrates spin-based critical sections and irq masking and was chosen because it gives a concrete anchor for explaining latency risks from spinning with interrupts disabled.
- `example_rwlock`: demonstrates reader/writer lock trade-offs under latency pressure and was chosen because it broadens the locking comparison beyond simple mutex-vs-spinlock narratives.

## Build steps

1. `cd lectures/Bai_7.1_PreemptRT/study_package/examples/example_mutex`
1. `make clean && make`
1. `cd lectures/Bai_7.1_PreemptRT/study_package/examples/example_spinlock`
1. `make clean && make`
1. `cd lectures/Bai_7.1_PreemptRT/study_package/examples/example_rwlock`
1. `make clean && make`

## Run steps

1. `cd lectures/Bai_7.1_PreemptRT/study_package`
1. `./run.sh`
1. Inspect `outputs/` for actual logs or expected-output files.

## Output

### example_mutex

Output file: `example_mutex.txt`

```text
Example: example_mutex
Status: executed

Build log:
make -C /lib/modules/6.12.47+rpt-rpi-v8/build M=/home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_7.1_PreemptRT/study_package/examples/example_mutex modules
make[1]: Entering directory '/usr/src/linux-headers-6.12.47+rpt-rpi-v8'
  CC [M]  /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_7.1_PreemptRT/study_package/examples/example_mutex/example_mutex.o
  MODPOST /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_7.1_PreemptRT/study_package/examples/example_mutex/Module.symvers
  CC [M]  /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_7.1_PreemptRT/study_package/examples/example_mutex/example_mutex.mod.o
  CC [M]  /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_7.1_PreemptRT/study_package/examples/example_mutex/.module-common.o
  LD [M]  /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_7.1_PreemptRT/study_package/examples/example_mutex/example_mutex.ko
make[1]: Leaving directory '/usr/src/linux-headers-6.12.47+rpt-rpi-v8'

Command log:
$ sudo insmod /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_7.1_PreemptRT/study_package/examples/example_mutex/example_mutex.ko
$ bash -lc sleep 1
$ sudo rmmod example_mutex

... [truncated]
```

### example_spinlock

Output file: `example_spinlock.txt`

```text
Example: example_spinlock
Status: executed

Build log:
make -C /lib/modules/6.12.47+rpt-rpi-v8/build M=/home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_7.1_PreemptRT/study_package/examples/example_spinlock modules
make[1]: Entering directory '/usr/src/linux-headers-6.12.47+rpt-rpi-v8'
  CC [M]  /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_7.1_PreemptRT/study_package/examples/example_spinlock/example_spinlock.o
  MODPOST /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_7.1_PreemptRT/study_package/examples/example_spinlock/Module.symvers
  CC [M]  /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_7.1_PreemptRT/study_package/examples/example_spinlock/example_spinlock.mod.o
  CC [M]  /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_7.1_PreemptRT/study_package/examples/example_spinlock/.module-common.o
  LD [M]  /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_7.1_PreemptRT/study_package/examples/example_spinlock/example_spinlock.ko
make[1]: Leaving directory '/usr/src/linux-headers-6.12.47+rpt-rpi-v8'

Command log:
$ sudo insmod /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_7.1_PreemptRT/study_package/examples/example_spinlock/example_spinlock.ko
$ bash -lc sleep 1
$ sudo rmmod example_spinlock

... [truncated]
```

### example_rwlock

Output file: `example_rwlock.txt`

```text
Example: example_rwlock
Status: executed

Build log:
make -C /lib/modules/6.12.47+rpt-rpi-v8/build M=/home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_7.1_PreemptRT/study_package/examples/example_rwlock modules
make[1]: Entering directory '/usr/src/linux-headers-6.12.47+rpt-rpi-v8'
  CC [M]  /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_7.1_PreemptRT/study_package/examples/example_rwlock/example_rwlock.o
  MODPOST /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_7.1_PreemptRT/study_package/examples/example_rwlock/Module.symvers
  CC [M]  /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_7.1_PreemptRT/study_package/examples/example_rwlock/example_rwlock.mod.o
  CC [M]  /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_7.1_PreemptRT/study_package/examples/example_rwlock/.module-common.o
  LD [M]  /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_7.1_PreemptRT/study_package/examples/example_rwlock/example_rwlock.ko
make[1]: Leaving directory '/usr/src/linux-headers-6.12.47+rpt-rpi-v8'

Command log:
$ sudo insmod /home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/lectures/Bai_7.1_PreemptRT/study_package/examples/example_rwlock/example_rwlock.ko
$ bash -lc sleep 1
$ sudo rmmod example_rwlock

... [truncated]
```

## Output explanation

- `example_mutex`: Kernel log shows successful mutex acquisition and release.
- `example_spinlock`: Kernel log shows lock and unlock events for static and dynamic spinlocks.
- `example_rwlock`: Kernel log distinguishes read-side and write-side lock acquisition.


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
