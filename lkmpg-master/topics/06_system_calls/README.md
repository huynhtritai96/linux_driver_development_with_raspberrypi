# System Calls Study Package

## Overview
This unit packages the LKMPG chapter topic **System Calls** into runnable or expected-output labs, backed by original repository source files and chapter references.

## Learning objectives
- Understand historical syscall-table hooking techniques.
- Recognize architecture and kernel-version fragility.
- Understand why this pattern is risky in production kernels.

## Concepts you must understand
- sys_call_table lookup
- kallsyms and kprobes fallback strategies
- write-protection manipulation assumptions
- ABI and arch-specific pt_regs layouts

## Key concepts
- sys_call_table lookup
- kallsyms and kprobes fallback strategies
- write-protection manipulation assumptions
- ABI and arch-specific pt_regs layouts

## Documentation and source files used
### Documentation files
- `lkmpg.tex (System Calls section)`
- `examples/syscall-steal.c`
### Source files used
- `examples/syscall-steal.c`

## Source mapping
| Source file | Why it matters |
|---|---|
| `examples/syscall-steal.c` | Provides core implementation details for this topic unit. |

## Example mapping
| Example | Demonstrates topic | Source file(s) | Original/adapted/reconstructed | Concept explained | Why selected | Runnable status |
|---|---|---|---|---|---|---|
| `syscall-steal` | System Calls | `examples/syscall-steal.c` | original | syscall table hooking risks | Important historical example and anti-pattern discussion. | `build_fail_arch_specific` |

## How each example relates to the topic
### `syscall-steal`
- Demonstrates: syscall table hooking risks
- Source: `examples/syscall-steal.c`
- Selection reason: Important historical example and anti-pattern discussion.
- Origin status: original
- Runnable status: `build_fail_arch_specific`
- Blocker: Build fails on ARM64 kernel 6.12.47+rpt-rpi-v8.

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
- `syscall-steal` -> `outputs/syscall-steal__expected_output.txt`

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
