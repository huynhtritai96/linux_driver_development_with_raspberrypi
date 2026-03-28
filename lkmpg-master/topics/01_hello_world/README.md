# Hello World Study Package

## Overview
This unit packages the LKMPG chapter topic **Hello World** into runnable or expected-output labs, backed by original repository source files and chapter references.

## Learning objectives
- Understand kernel module init/exit lifecycle.
- Build out-of-tree modules with Kbuild.
- Use module metadata and module parameters safely.
- Understand multi-file module linking with startstop-objs.

## Concepts you must understand
- init_module vs module_init/module_exit
- __init and __exit annotations
- MODULE_LICENSE and module metadata macros
- module_param and module_param_array
- multi-object module composition

## Key concepts
- init_module vs module_init/module_exit
- __init and __exit annotations
- MODULE_LICENSE and module metadata macros
- module_param and module_param_array
- multi-object module composition

## Documentation and source files used
### Documentation files
- `README.md`
- `lkmpg.tex (Hello World section)`
- `examples/Makefile`
### Source files used
- `examples/hello-1.c`
- `examples/hello-2.c`
- `examples/hello-3.c`
- `examples/hello-4.c`
- `examples/hello-5.c`
- `examples/start.c`
- `examples/stop.c`

## Source mapping
| Source file | Why it matters |
|---|---|
| `examples/hello-1.c` | Provides core implementation details for this topic unit. |
| `examples/hello-2.c` | Provides core implementation details for this topic unit. |
| `examples/hello-3.c` | Provides core implementation details for this topic unit. |
| `examples/hello-4.c` | Provides core implementation details for this topic unit. |
| `examples/hello-5.c` | Provides core implementation details for this topic unit. |
| `examples/start.c` | Provides core implementation details for this topic unit. |
| `examples/stop.c` | Provides core implementation details for this topic unit. |

## Example mapping
| Example | Demonstrates topic | Source file(s) | Original/adapted/reconstructed | Concept explained | Why selected | Runnable status |
|---|---|---|---|---|---|---|
| `hello-1` | Hello World | `examples/hello-1.c` | original | module lifecycle basics | First runnable baseline for kernel module loading. | `directly_runnable` |
| `hello-2` | Hello World | `examples/hello-2.c` | original | preferred init/exit registration | Shows modern lifecycle macros replacing legacy style. | `directly_runnable` |
| `hello-3` | Hello World | `examples/hello-3.c` | original | init/exit section annotations | Teaches memory lifetime hints for module code. | `directly_runnable` |
| `hello-4` | Hello World | `examples/hello-4.c` | original | module metadata and licensing | Connects source metadata to modinfo behavior. | `directly_runnable` |
| `hello-5` | Hello World | `examples/hello-5.c` | original | module_param interfaces | Shows user-configurable module state at load time. | `directly_runnable` |
| `startstop` | Hello World | `examples/start.c`, `examples/stop.c` | original | modular compilation units | Shows scaling beyond a single source file. | `directly_runnable` |

## How each example relates to the topic
### `hello-1`
- Demonstrates: module lifecycle basics
- Source: `examples/hello-1.c`
- Selection reason: First runnable baseline for kernel module loading.
- Origin status: original
- Runnable status: `directly_runnable`

### `hello-2`
- Demonstrates: preferred init/exit registration
- Source: `examples/hello-2.c`
- Selection reason: Shows modern lifecycle macros replacing legacy style.
- Origin status: original
- Runnable status: `directly_runnable`

### `hello-3`
- Demonstrates: init/exit section annotations
- Source: `examples/hello-3.c`
- Selection reason: Teaches memory lifetime hints for module code.
- Origin status: original
- Runnable status: `directly_runnable`

### `hello-4`
- Demonstrates: module metadata and licensing
- Source: `examples/hello-4.c`
- Selection reason: Connects source metadata to modinfo behavior.
- Origin status: original
- Runnable status: `directly_runnable`

### `hello-5`
- Demonstrates: module_param interfaces
- Source: `examples/hello-5.c`
- Selection reason: Shows user-configurable module state at load time.
- Origin status: original
- Runnable status: `directly_runnable`

### `startstop`
- Demonstrates: modular compilation units
- Source: `examples/start.c`, `examples/stop.c`
- Selection reason: Shows scaling beyond a single source file.
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
- `hello-1` -> `outputs/hello-1.txt`
- `hello-2` -> `outputs/hello-2.txt`
- `hello-3` -> `outputs/hello-3.txt`
- `hello-4` -> `outputs/hello-4.txt`
- `hello-5` -> `outputs/hello-5.txt`
- `startstop` -> `outputs/startstop.txt`

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
