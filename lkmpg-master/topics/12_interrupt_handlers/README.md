# Interrupt Handlers Study Package

## Overview
This unit packages the LKMPG chapter topic **Interrupt Handlers** into runnable or expected-output labs, backed by original repository source files and chapter references.

## Learning objectives
- Register and free GPIO-backed IRQs.
- Compare plain ISR, workqueue bottom-half, and threaded IRQ models.
- Understand why hardware setup determines runtime viability.

## Concepts you must understand
- request_irq/free_irq
- request_threaded_irq + IRQ_WAKE_THREAD
- fast ISR vs deferred processing
- GPIO-to-IRQ mapping

## Key concepts
- request_irq/free_irq
- request_threaded_irq + IRQ_WAKE_THREAD
- fast ISR vs deferred processing
- GPIO-to-IRQ mapping

## Documentation and source files used
### Documentation files
- `lkmpg.tex (Interrupt Handlers section)`
- `examples/intrpt.c`
- `examples/bottomhalf.c`
- `examples/bh_threaded.c`
### Source files used
- `examples/bh_threaded.c`
- `examples/bottomhalf.c`
- `examples/intrpt.c`

## Source mapping
| Source file | Why it matters |
|---|---|
| `examples/bh_threaded.c` | Provides core implementation details for this topic unit. |
| `examples/bottomhalf.c` | Provides core implementation details for this topic unit. |
| `examples/intrpt.c` | Provides core implementation details for this topic unit. |

## Example mapping
| Example | Demonstrates topic | Source file(s) | Original/adapted/reconstructed | Concept explained | Why selected | Runnable status |
|---|---|---|---|---|---|---|
| `intrpt` | Interrupt Handlers | `examples/intrpt.c` | original | top-half IRQ callback | Core interrupt registration and ISR flow. | `needs_hardware` |
| `bottomhalf` | Interrupt Handlers | `examples/bottomhalf.c` | original | split top-half and workqueue bottom-half | Illustrates short ISR + deferred heavy work. | `needs_hardware` |
| `bh_threaded` | Interrupt Handlers | `examples/bh_threaded.c` | original | request_threaded_irq flow | Demonstrates kernel-managed threaded interrupt bottom half. | `needs_hardware` |

## How each example relates to the topic
### `intrpt`
- Demonstrates: top-half IRQ callback
- Source: `examples/intrpt.c`
- Selection reason: Core interrupt registration and ISR flow.
- Origin status: original
- Runnable status: `needs_hardware`
- Blocker: GPIO request fails on this host before IRQ setup.

### `bottomhalf`
- Demonstrates: split top-half and workqueue bottom-half
- Source: `examples/bottomhalf.c`
- Selection reason: Illustrates short ISR + deferred heavy work.
- Origin status: original
- Runnable status: `needs_hardware`
- Blocker: GPIO request fails on this host before IRQ setup.

### `bh_threaded`
- Demonstrates: request_threaded_irq flow
- Source: `examples/bh_threaded.c`
- Selection reason: Demonstrates kernel-managed threaded interrupt bottom half.
- Origin status: original
- Runnable status: `needs_hardware`
- Blocker: GPIO request fails on this host before IRQ setup.

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
- `intrpt` -> `outputs/intrpt__expected_output.txt`
- `bottomhalf` -> `outputs/bottomhalf__expected_output.txt`
- `bh_threaded` -> `outputs/bh_threaded__expected_output.txt`

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
