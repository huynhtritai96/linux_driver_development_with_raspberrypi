# Manual Review

## Incomplete or adaptation-prone examples

- `devicemodel`: platform driver lifecycle demonstrates registration, but meaningful probe requires a matching platform device.
- `devicetree`: module loads cleanly; probe path requires compatible DT nodes (or loaded overlay).
- `vkbd`: requires build/link adaptation (`KBUILD_EXTRA_SYMBOLS`) and `vinput` to be loaded first.

## Version-sensitive examples

- `syscall-steal`: build-break on this ARM64 kernel due x86-oriented assumptions (`read_cr0`) and register-layout differences (`pt_regs.si`).
- `kbleds`: behavior depends on console/tty environment and may vary across systems.
- `print_string`: direct tty writes depend on current task tty availability.

## Examples requiring hardware or unsupported environment

- `intrpt`, `bottomhalf`, `bh_threaded`: require board-specific GPIO buttons/LED wiring and IRQ-capable GPIO lines.
- `led`: requires available output GPIO line and connected LED.
- `dht11`: requires physical DHT11 sensor and correct pin/timing assumptions.
- `dt-overlay`: requires overlay-capable firmware/kernel pipeline and board-specific deployment workflow.

## Unclear topic mappings

- Chapter organization is document-driven (`lkmpg.tex`) while source layout is flat (`examples/`), so chapter folder boundaries are generated rather than native.
- Some topics (Device Model, Device Tree) are conceptually complete but runtime-observable behaviors depend on external platform state.
