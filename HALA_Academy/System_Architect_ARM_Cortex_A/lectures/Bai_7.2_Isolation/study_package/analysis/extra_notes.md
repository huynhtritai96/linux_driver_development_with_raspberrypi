# Extra Notes

## Representative lecture commands

- `echo 3 > "$irq/smp_affinity" 2>/dev/null`
- `echo "=== Starting CPU Isolation Setup ==="`
- `echo "[*] Stopping irqbalance..."`
- `echo "[*] Migrating IRQs to CPUs $NON_RT_CPUS..."`
- `echo 3 > "$irq/smp_affinity" 2>/dev/null`
- `echo "Checking RCU offload..."`
- `echo "=== Verification ==="`
- `echo "Interrupts on isolated CPUs:"`
- `cat /proc/interrupts | grep -E "CPU2|CPU3" | head -n 5`
- `echo "Done. Run your RT app with: taskset -c 2-3 "`
- `mount -t cgroup2 none /sys/fs/cgroup`
- `echo "2-3" > /sys/fs/cgroup/rt_domain/cpuset.cpus`

## Figures and diagrams seen in the lecture

- No figure captions were detected in the parsed lecture Markdown.

## Mapping note

CPU affinity and nohz tuning are scheduler configuration topics, not module examples in LKMPG.

## Gap note

No close LKMPG example exists for cpu isolation, housekeeping cores, or nohz_full tuning.
