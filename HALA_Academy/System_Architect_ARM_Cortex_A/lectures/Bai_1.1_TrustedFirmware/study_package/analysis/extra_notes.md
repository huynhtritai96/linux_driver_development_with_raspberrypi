# Extra Notes

## Representative lecture commands

- `sudo apt update`
- `sudo apt install gcc-aarch64-linux-gnu make git`
- `make CROSS_COMPILE=aarch64-linux-gnu- PLAT=sun50i_h616 DEBUG=1 bl31`

## Figures and diagrams seen in the lecture

- Hình 1: Quy trình Boot Chain
- Hình 2: Kiến trúc TF-A

## Mapping note

LKMPG starts at kernel module scope and does not cover secure monitor firmware or BL31/BL32 handoff.

## Gap note

No close LKMPG example exists for TF-A or EL3 firmware sequencing; keep this lecture analysis-heavy and flag manual review.
