# Pitfalls: Talking To Device Files

## Common failure modes
- Kernel/header mismatch when building external modules.
- Missing root privileges for module operations and privileged file writes.
- Assuming board-specific GPIO/DT/IRQ resources exist without verification.
- Neglecting dependency order between modules (for example, `vkbd` depending on `vinput`).

## Version and environment sensitivity
- API behavior may differ across kernel versions; compatibility guards must be checked in source.
- Architecture assumptions can break builds (`syscall-steal` on this ARM64 environment).
- Hardware and firmware state can make examples partially runnable even when build succeeds.

## Debugging implications
- Always correlate shell output with kernel logs.
- Keep repro steps minimal and deterministic when triaging failures.
- Validate cleanup success to avoid stale module/device state between runs.
