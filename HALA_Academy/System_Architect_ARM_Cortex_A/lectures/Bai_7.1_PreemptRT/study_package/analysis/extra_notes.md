# Extra Notes

## Representative lecture commands

- `echo wakeup_rt > /sys/kernel/debug/tracing/current_tracer`
- `echo 1 > /sys/kernel/debug/tracing/tracing_on`
- `cyclictest -p 90 -m -t1 -n -i 1000 -l 10000`
- `cat /sys/kernel/debug/tracing/trace | head -n 20`
- `sudo apt install rt-tests`
- `sudo hwlatdetect --duration=60 --threshold=10`
- `echo "Starting Hardware Latency Detector..."`
- `echo "Please do not touch the system..."`
- `sudo hwlatdetect \`
- `cat hwlat_report.txt`
- `echo "Check hwlat_report.txt for details."`
- `echo "No latency spikes detected above threshold."`

## Figures and diagrams seen in the lecture

- Hình 1: Sự khác biệt trong xử lý ngắt và critical section

## Mapping note

LKMPG cannot patch the kernel into PREEMPT_RT, but it does provide the locking primitives the lecture analyzes.

## Gap note

The examples explain lock behavior, not the full PREEMPT_RT patchset or tracing tooling.
