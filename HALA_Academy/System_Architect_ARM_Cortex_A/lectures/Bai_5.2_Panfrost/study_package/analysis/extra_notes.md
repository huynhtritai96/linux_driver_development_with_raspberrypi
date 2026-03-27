# Extra Notes

## Representative lecture commands

- `cat /sys/class/devfreq/1800000.gpu/governor`
- `echo performance > /sys/class/devfreq/1800000.gpu/governor`
- `echo 50 > /sys/class/devfreq/1800000.gpu/load_threshold`

## Figures and diagrams seen in the lecture

- Hình 1: Mali Job Manager Flow

## Mapping note

GPU job scheduling and devfreq policies are not covered by LKMPG.

## Gap note

No close LKMPG example exists for Panfrost, Mali job manager internals, or GPU DVFS.
