# Extra Notes

## Representative lecture commands

- `gst-launch-1.0 filesrc location=video.mp4 ! \`
- `perf stat -e arm_dsu_0/bus_access/ -a sleep 5`

## Figures and diagrams seen in the lecture

- Hình 1: Pipeline không copy dữ liệu giữa VPU và Display
- Hình 2: Luồng dữ liệu nén AFBC trong SoC
- Hình 3: Cơ chế dma_fence giúp CPU không bao giờ bị block

## Mapping note

DMA-BUF and multimedia fence pipelines are not represented in LKMPG.

## Gap note

No close LKMPG example exists for zero-copy VPU-to-display paths, AFBC, or dma_fence coordination.
