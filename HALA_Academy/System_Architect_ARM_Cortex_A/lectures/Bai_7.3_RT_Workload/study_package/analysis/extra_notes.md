# Extra Notes

## Representative lecture commands

- `echo "Starting Stress-ng (Background load)..."`
- `stress-ng --cpu 2 --io 2 --vm 1 --vm-bytes 128M --timeout 60s &`
- `echo "Starting Cyclictest on CPU 3 (Priority 99)..."`
- `sudo cyclictest \`
- `echo "Max Latency measured: ${MAX_LAT} us"`
- `echo "PASS: Hard Real-Time Ready!"`
- `echo "FAIL: Optimize more!"`

## Figures and diagrams seen in the lecture

- Hình 1: Cơ chế Priority Inheritance giải quyết vấn đề

## Mapping note

LKMPG does not provide RT user-space threads, but it does provide the key kernel-side synchronization primitives.

## Gap note

Full RT workload shaping and lock-free queues are only partially represented; the package calls that out explicitly.
