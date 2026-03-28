# Extra Notes: Replacing Print Macros

## Environment used for this package
- Kernel: `6.12.47+rpt-rpi-v8`
- Architecture: `aarch64`
- Privilege model: passwordless `sudo` available
- Headers: `/lib/modules/$(uname -r)/build` present

## Adaptations and constraints
- Original LKMPG source logic is preserved; no behavioral code rewrites were introduced in source examples.
- Build/run orchestration is provided via generated topic `run.sh` scripts.
- Blocked examples are represented with expected-output files and explicit reasons.

## Verification notes
- Executed examples capture both command output and dmesg deltas per run.
- Topic status files (`outputs/_run_status.tsv`) are used by root summaries for traceable counts.
