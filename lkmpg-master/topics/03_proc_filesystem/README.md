# The /proc Filesystem Study Package

## Overview
This unit packages the LKMPG chapter topic **The /proc Filesystem** into runnable or expected-output labs, backed by original repository source files and chapter references.

## Learning objectives
- Create and remove proc entries safely.
- Implement read/write callbacks for proc endpoints.
- Understand when seq_file is preferred for iteration/scalable output.

## Concepts you must understand
- proc_create / proc_remove / remove_proc_entry
- proc_ops compatibility guards
- copy_from_user and buffer bounds
- seq_file iterator callbacks

## Key concepts
- proc_create / proc_remove / remove_proc_entry
- proc_ops compatibility guards
- copy_from_user and buffer bounds
- seq_file iterator callbacks

## Documentation and source files used
### Documentation files
- `lkmpg.tex (The /proc Filesystem section)`
- `examples/procfs1.c`
- `examples/procfs2.c`
- `examples/procfs3.c`
- `examples/procfs4.c`
### Source files used
- `examples/procfs1.c`
- `examples/procfs2.c`
- `examples/procfs3.c`
- `examples/procfs4.c`

## Source mapping
| Source file | Why it matters |
|---|---|
| `examples/procfs1.c` | Provides core implementation details for this topic unit. |
| `examples/procfs2.c` | Provides core implementation details for this topic unit. |
| `examples/procfs3.c` | Provides core implementation details for this topic unit. |
| `examples/procfs4.c` | Provides core implementation details for this topic unit. |

## Example mapping
| Example | Demonstrates topic | Source file(s) | Original/adapted/reconstructed | Concept explained | Why selected | Runnable status |
|---|---|---|---|---|---|---|
| `procfs1` | The /proc Filesystem | `examples/procfs1.c` | original | proc_create + read callback | Fast path into procfs API basics. | `directly_runnable` |
| `procfs2` | The /proc Filesystem | `examples/procfs2.c` | original | copy_from_user/copy_to_user via procfs | Teaches safe user-kernel buffer handling. | `directly_runnable` |
| `procfs3` | The /proc Filesystem | `examples/procfs3.c` | original | proc file lifecycle and ownership metadata | Bridges procfs with familiar file-style handlers. | `directly_runnable` |
| `procfs4` | The /proc Filesystem | `examples/procfs4.c` | original | seq_file iteration | Shows scalable /proc output model. | `directly_runnable` |

## How each example relates to the topic
### `procfs1`
- Demonstrates: proc_create + read callback
- Source: `examples/procfs1.c`
- Selection reason: Fast path into procfs API basics.
- Origin status: original
- Runnable status: `directly_runnable`

### `procfs2`
- Demonstrates: copy_from_user/copy_to_user via procfs
- Source: `examples/procfs2.c`
- Selection reason: Teaches safe user-kernel buffer handling.
- Origin status: original
- Runnable status: `directly_runnable`

### `procfs3`
- Demonstrates: proc file lifecycle and ownership metadata
- Source: `examples/procfs3.c`
- Selection reason: Bridges procfs with familiar file-style handlers.
- Origin status: original
- Runnable status: `directly_runnable`

### `procfs4`
- Demonstrates: seq_file iteration
- Source: `examples/procfs4.c`
- Selection reason: Shows scalable /proc output model.
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
- `procfs1` -> `outputs/procfs1.txt`
- `procfs2` -> `outputs/procfs2.txt`
- `procfs3` -> `outputs/procfs3.txt`
- `procfs4` -> `outputs/procfs4.txt`

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
