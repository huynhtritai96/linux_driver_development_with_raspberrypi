# Extra Notes

## Representative lecture commands

- `perf sched record -- sleep 5`
- `perf sched latency --sort max`
- `perf sched map`
- `mount -t cgroup2 none /sys/fs/cgroup`
- `echo "50000 100000" > /sys/fs/cgroup/limit_g/cpu.max`
- `echo 134217728 > /sys/fs/cgroup/limit_g/memory.max`
- `echo $PID > /sys/fs/cgroup/limit_g/cgroup.procs`
- `cat /proc/sys/kernel/sched_migration_cost_ns`
- `cat /proc/sys/kernel/sched_rt_period_us`
- `cat /proc/sys/kernel/sched_rt_runtime_us`

## Figures and diagrams seen in the lecture

- Hình 1: Scheduling Context Switch & Latency Analysis
- Hình 2: Các Kernel Preemption Models

## Mapping note

LKMPG has clean runnable examples for workqueues, completions, and deferred work.

## Gap note

cgroups v2 and sysctl tuning remain outside LKMPG and are documented as lecture-only material.
