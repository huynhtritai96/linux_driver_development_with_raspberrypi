# Blocking Processes and threads Study Package

## Overview
This unit packages the LKMPG chapter topic **Blocking Processes and threads** into runnable or expected-output labs, backed by original repository source files and chapter references.

## Learning objectives
- Implement wait-queue based blocking behavior.
- Observe non-blocking O_NONBLOCK behavior from userspace.
- Use completion primitives for deterministic ordering.

## Concepts you must understand
- wait_event_interruptible
- EAGAIN behavior for O_NONBLOCK
- completion lifecycle (init/complete/wait)
- kthread completion handoff

## Key concepts
- wait_event_interruptible
- EAGAIN behavior for O_NONBLOCK
- completion lifecycle (init/complete/wait)
- kthread completion handoff

## Documentation and source files used
### Documentation files
- `lkmpg.tex (Blocking Processes and threads section)`
- `examples/sleep.c`
- `examples/completions.c`
- `examples/other/cat_nonblock.c`
### Source files used
- `examples/completions.c`
- `examples/other/cat_nonblock.c`
- `examples/sleep.c`

## Source mapping
| Source file | Why it matters |
|---|---|
| `examples/completions.c` | Provides core implementation details for this topic unit. |
| `examples/other/cat_nonblock.c` | Provides core implementation details for this topic unit. |
| `examples/sleep.c` | Provides core implementation details for this topic unit. |

## Example mapping
| Example | Demonstrates topic | Source file(s) | Original/adapted/reconstructed | Concept explained | Why selected | Runnable status |
|---|---|---|---|---|---|---|
| `sleep` | Blocking Processes and threads | `examples/sleep.c` | original | blocking semantics and wakeups | Practical concurrency model for file open arbitration. | `directly_runnable` |
| `cat_nonblock` | Blocking Processes and threads | `examples/other/cat_nonblock.c` | original | non-blocking userspace behavior | Shows user-observable consequence of kernel wait-queue logic. | `directly_runnable_with_module` |
| `completions` | Blocking Processes and threads | `examples/completions.c` | original | one-shot synchronization points | Clear example for ordering asynchronous threads. | `directly_runnable` |

## How each example relates to the topic
### `sleep`
- Demonstrates: blocking semantics and wakeups
- Source: `examples/sleep.c`
- Selection reason: Practical concurrency model for file open arbitration.
- Origin status: original
- Runnable status: `directly_runnable`

### `cat_nonblock`
- Demonstrates: non-blocking userspace behavior
- Source: `examples/other/cat_nonblock.c`
- Selection reason: Shows user-observable consequence of kernel wait-queue logic.
- Origin status: original
- Runnable status: `directly_runnable_with_module`

### `completions`
- Demonstrates: one-shot synchronization points
- Source: `examples/completions.c`
- Selection reason: Clear example for ordering asynchronous threads.
- Origin status: original
- Runnable status: `directly_runnable`

## Build steps
1. Change to `<lkmpg-root>/examples`.
2. Build each module with: `make -C /lib/modules/$(uname -r)/build M=$(pwd) <module>.ko`.
3. Build userspace helpers with `gcc -O2 -Wall -Wextra` where relevant.
4. Or run all topic steps using this package script: `./run.sh`.

## Run steps
1. Change to this study package directory.
2. Execute `./run.sh`.
3. Review `outputs/*.txt` and `outputs/*__expected_output.txt`.
4. Use `sudo dmesg | tail -n 120` for extra kernel-side context.

## Output
- `sleep` -> `outputs/sleep.txt`
- `cat_nonblock` -> `outputs/cat_nonblock.txt`
- `completions` -> `outputs/completions.txt`

## Output explanation
Each output file separates command-level behavior from kernel log deltas. For blocked examples, expected-output files document why execution is not claimed as successful.

## Detailed code analysis
Focus points include init/exit sequencing, callback registration, userspace copy boundaries, and cleanup paths. See `analysis/code_walkthrough.md` for per-example walkthrough details.

## Kernel / system behavior analysis
This unit explicitly distinguishes kernel-space logic (module callbacks, synchronization, registration APIs) from user-space actions (shell commands, helper binaries, file I/O). It also highlights object lifetime, init/cleanup flow, and concurrency implications for these examples.

## Common pitfalls
- Running module examples without matching kernel headers for the active kernel.
- Forgetting root privileges for module insertion/removal or privileged sysfs/procfs writes.
- Assuming hardware-facing samples are portable without pin remapping and wiring validation.
- Ignoring cleanup order, which can leave stale device nodes or in-kernel resources.

## Common mistakes
- Running module examples without matching kernel headers for the active kernel.
- Forgetting root privileges for module insertion/removal or privileged sysfs/procfs writes.
- Assuming hardware-facing samples are portable without pin remapping and wiring validation.
- Ignoring cleanup order, which can leave stale device nodes or in-kernel resources.

## Debugging tips
- Always inspect recent kernel logs with `sudo dmesg | tail -n 120` after each run.
- Use `lsmod`, `/proc/devices`, and `/sys/class` to confirm registration side effects.
- Rebuild single modules with explicit Kbuild targets (`<module>.ko`) to isolate failures.
- For ABI mismatches, compare headers and kernel version guards in source code.

## Deeper analysis
Why the concept matters: the selected examples show concrete kernel engineering tradeoffs around safety, lifecycle, version compatibility, and runtime observability. Hardware- and platform-dependent units are marked conservatively to avoid false claims of reproducibility.

## Suggested exercises
- Modify parameters or input values and compare resulting kernel logs.
- Add additional sanity checks to userspace helper flows and observe error paths.
- Trace init/exit and callback ordering for each example.
- Document one additional edge case per example and reproduce it safely.

## Suggested next steps
1. Re-run this package with custom inputs and compare output deltas.
2. Extend one example with additional validation logging and document behavioral changes.
3. Compare this unit with adjacent topic units to understand API progression across chapters.
