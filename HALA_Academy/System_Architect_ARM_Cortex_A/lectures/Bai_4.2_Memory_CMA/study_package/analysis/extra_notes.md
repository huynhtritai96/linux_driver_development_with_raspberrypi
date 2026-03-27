# Extra Notes

## Representative lecture commands

- `cat /proc/slab_allocators`
- `cat /proc/buddyinfo`
- `mount -t debugfs none /sys/kernel/debug`
- `cat /sys/kernel/debug/dma_buf/bufinfo`

## Figures and diagrams seen in the lecture

- Hình 1: DMA-BUF Heaps System vs CMA
- Hình 2: Kernel/Movable Pages & CMA Region

## Mapping note

The guide does not cover CMA, DMA-BUF, or kernel allocator diagnostics in depth.

## Gap note

No close LKMPG example exists for CMA regions, DMA-BUF heaps, or allocator profiling.
