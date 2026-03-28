# Synchronization Study Package

## Overview
This unit packages the LKMPG chapter topic **Synchronization** into runnable or expected-output labs, backed by original repository source files and chapter references.

## Learning objectives
- Choose the right synchronization primitive for context.
- Compare sleepable vs non-sleepable lock types.
- Understand atomic operations as lock-free alternatives.

## Concepts you must understand
- mutex APIs
- spin_lock_irqsave patterns
- rwlock read/write semantics
- atomic_t operations and bit ops

## Key concepts
- mutex APIs
- spin_lock_irqsave patterns
- rwlock read/write semantics
- atomic_t operations and bit ops

## Documentation and source files used
### Documentation files
- `lkmpg.tex (Synchronization section)`
- `examples/example_mutex.c`
- `examples/example_spinlock.c`
- `examples/example_rwlock.c`
- `examples/example_atomic.c`
### Source files used
- `examples/example_atomic.c`
- `examples/example_mutex.c`
- `examples/example_rwlock.c`
- `examples/example_spinlock.c`

## Source mapping
| Source file | Why it matters |
|---|---|
| `examples/example_atomic.c` | Provides core implementation details for this topic unit. |
| `examples/example_mutex.c` | Provides core implementation details for this topic unit. |
| `examples/example_rwlock.c` | Provides core implementation details for this topic unit. |
| `examples/example_spinlock.c` | Provides core implementation details for this topic unit. |

## Example mapping
| Example | Demonstrates topic | Source file(s) | Original/adapted/reconstructed | Concept explained | Why selected | Runnable status |
|---|---|---|---|---|---|---|
| `example_mutex` | Synchronization | `examples/example_mutex.c` | original | sleepable lock semantics | Baseline lock type for process-context code. | `directly_runnable` |
| `example_spinlock` | Synchronization | `examples/example_spinlock.c` | original | atomic-context locking | Contrasts with mutex behavior under interrupt concerns. | `directly_runnable` |
| `example_rwlock` | Synchronization | `examples/example_rwlock.c` | original | read-mostly lock strategy | Shows lock mode split for concurrent readers. | `directly_runnable` |
| `example_atomic` | Synchronization | `examples/example_atomic.c` | original | lock-free scalar updates | Teaches when atomic_t is sufficient. | `directly_runnable` |

## How each example relates to the topic
### `example_mutex`
- Demonstrates: sleepable lock semantics
- Source: `examples/example_mutex.c`
- Selection reason: Baseline lock type for process-context code.
- Origin status: original
- Runnable status: `directly_runnable`

### `example_spinlock`
- Demonstrates: atomic-context locking
- Source: `examples/example_spinlock.c`
- Selection reason: Contrasts with mutex behavior under interrupt concerns.
- Origin status: original
- Runnable status: `directly_runnable`

### `example_rwlock`
- Demonstrates: read-mostly lock strategy
- Source: `examples/example_rwlock.c`
- Selection reason: Shows lock mode split for concurrent readers.
- Origin status: original
- Runnable status: `directly_runnable`

### `example_atomic`
- Demonstrates: lock-free scalar updates
- Source: `examples/example_atomic.c`
- Selection reason: Teaches when atomic_t is sufficient.
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
- `example_mutex` -> `outputs/example_mutex.txt`
- `example_spinlock` -> `outputs/example_spinlock.txt`
- `example_rwlock` -> `outputs/example_rwlock.txt`
- `example_atomic` -> `outputs/example_atomic.txt`

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
