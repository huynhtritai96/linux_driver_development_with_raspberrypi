# Virtual Input Device Driver Study Package

## Overview
This unit packages the LKMPG chapter topic **Virtual Input Device Driver** into runnable or expected-output labs, backed by original repository source files and chapter references.

## Learning objectives
- Understand virtual input framework design in LKMPG examples.
- Handle inter-module symbol dependencies for out-of-tree builds.
- Create and remove virtual keyboard instances via sysfs.

## Concepts you must understand
- class attributes export/unexport
- input subsystem registration
- EXPORT_SYMBOL and Module.symvers dependency
- event injection via /dev/vinputN

## Key concepts
- class attributes export/unexport
- input subsystem registration
- EXPORT_SYMBOL and Module.symvers dependency
- event injection via /dev/vinputN

## Documentation and source files used
### Documentation files
- `lkmpg.tex (Virtual Input Device Driver section)`
- `examples/vinput.c`
- `examples/vkbd.c`
- `examples/vinput.h`
### Source files used
- `examples/vinput.c`
- `examples/vinput.h`
- `examples/vkbd.c`

## Source mapping
| Source file | Why it matters |
|---|---|
| `examples/vinput.c` | Provides core implementation details for this topic unit. |
| `examples/vinput.h` | Provides core implementation details for this topic unit. |
| `examples/vkbd.c` | Provides core implementation details for this topic unit. |

## Example mapping
| Example | Demonstrates topic | Source file(s) | Original/adapted/reconstructed | Concept explained | Why selected | Runnable status |
|---|---|---|---|---|---|---|
| `vinput` | Virtual Input Device Driver | `examples/vinput.c`, `examples/vinput.h` | original | registering virtual input framework and class attributes | Base layer required by vkbd extension. | `directly_runnable` |
| `vkbd` | Virtual Input Device Driver | `examples/vkbd.c`, `examples/vinput.h`, `examples/vinput.c` | original (build adaptation only) | cross-module symbol dependency in out-of-tree modules | Shows realistic multi-module integration workflow. | `runnable_with_build_adaptation` |

## How each example relates to the topic
### `vinput`
- Demonstrates: registering virtual input framework and class attributes
- Source: `examples/vinput.c`, `examples/vinput.h`
- Selection reason: Base layer required by vkbd extension.
- Origin status: original
- Runnable status: `directly_runnable`

### `vkbd`
- Demonstrates: cross-module symbol dependency in out-of-tree modules
- Source: `examples/vkbd.c`, `examples/vinput.h`, `examples/vinput.c`
- Selection reason: Shows realistic multi-module integration workflow.
- Origin status: original (build adaptation only)
- Runnable status: `runnable_with_build_adaptation`

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
- `vinput` -> `outputs/vinput.txt`
- `vkbd` -> `outputs/vkbd.txt`

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
