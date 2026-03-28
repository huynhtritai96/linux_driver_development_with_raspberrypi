# Mermaid Integration Report

## Summary

- raw Markdown files considered: 21
- raw Markdown files updated: 19
- raw Markdown files with no mapped Mermaid: 2
- Mermaid files mapped by manifest: 27
- Mermaid blocks inserted inline: 27
- unmatched Mermaid files: 16
- uncovered figure captions: 1
- anchor failures: 0

## Inserted Mappings

| key | markdown file | anchor | Mermaid files | status | notes |
| --- | --- | --- | --- | --- | --- |
| bai_1_1_hinh_1 | Bai_1.1_TrustedFirmware__raw.md | *Hình 1: Quy trình Boot Chain* | 1.1_boot_chain_detail__d1__en.mmd | inserted | Inserted Mermaid block(s) inline after anchor. |
| bai_1_1_hinh_2 | Bai_1.1_TrustedFirmware__raw.md | *Hình 2: Kiến trúc TF-A* | 1.1_tfa_internals__d1__en.mmd, 1.1_tfa_internals__d2__en.mmd, 1.1_tfa_internals__d3__en.mmd | inserted | Inserted Mermaid block(s) inline after anchor. |
| bai_1_2_hinh_1 | Bai_1.2_UBoot__raw.md | *Hình 1: Kiến trúc bộ nhớ U-Boot* | 1.2_uboot_memory_layout__d1__en.mmd | inserted | Inserted Mermaid block(s) inline after anchor. |
| bai_2_1_hinh_1 | Bai_2.1_Kernel_DTB__raw.md | *Hình 1: Luồng khởi động Kernel* | Week1_kernel_boot_flow__d1__en.mmd | inserted | Inserted Mermaid block(s) inline after anchor. |
| bai_2_2_hinh_1 | Bai_2.2_Initramfs__raw.md | *Hình 1: Luồng boot với Initramfs* | 2.2_initramfs_flow__d1__en.mmd | inserted | Inserted Mermaid block(s) inline after anchor. |
| bai_2_3_hinh_1 | Bai_2.3_Buildroot__raw.md | *Hình 1: Systemd Service Graph* | 2.3_service_graph__d1__en.mmd | inserted | Inserted Mermaid block(s) inline after anchor. |
| bai_3_1_hinh_1 | Bai_3.1_DeviceTree__raw.md | *Hình 1: Cấu trúc Device Tree* | 3.1_dt_structure__d1__en.mmd | inserted | Inserted Mermaid block(s) inline after anchor. |
| bai_3_2_hinh_1 | Bai_3.2_Touchscreen__raw.md | *Hình 1: Kiến trúc Input Subsystem* | 3.2_input_subsystem__d1__en.mmd | inserted | Inserted Mermaid block(s) inline after anchor. |
| bai_3_3_hinh_1 | Bai_3.3_Sensors__raw.md | *Hình 1: Kiến trúc IIO Subsystem* | 3.3_iio_subsystem__d1__en.mmd | inserted | Inserted Mermaid block(s) inline after anchor. |
| bai_4_1_hinh_1 | Bai_4.1_BootProfiling__raw.md | *Hình 1: U-Boot Bootstage Timeline* | 4.1_bootstage_timeline__d1__en.mmd | inserted | Inserted Mermaid block(s) inline after anchor. |
| bai_4_1_hinh_2 | Bai_4.1_BootProfiling__raw.md | *Hình 2: Quy trình Phân tích và Tối ưu Boot Time* | 4.1_boot_analysis_flow__d1__en.mmd | inserted | Inserted Mermaid block(s) inline after anchor. |
| bai_4_2_hinh_1 | Bai_4.2_Memory_CMA__raw.md | *Hình 1: DMA-BUF Heaps System vs CMA* | 4.2_dma_buf_heaps__d1__en.mmd | inserted | Inserted Mermaid block(s) inline after anchor. |
| bai_4_2_hinh_2 | Bai_4.2_Memory_CMA__raw.md | *Hình 2: Kernel/Movable Pages & CMA Region* | 4.2_cma_architecture__d1__en.mmd | inserted | Inserted Mermaid block(s) inline after anchor. |
| bai_4_3_hinh_1 | Bai_4.3_Scheduler__raw.md | *Hình 1: Scheduling Context Switch & Latency Analysis* | 4.3_perf_sched_latency__d1__en.mmd | inserted | Inserted Mermaid block(s) inline after anchor. |
| bai_4_3_hinh_2 | Bai_4.3_Scheduler__raw.md | *Hình 2: Các Kernel Preemption Models* | 4.3_preempt_models__d1__en.mmd | inserted | Inserted Mermaid block(s) inline after anchor. |
| bai_5_1_hinh_1 | Bai_5.1_DRM_KMS__raw.md | *Hình 1: Hardware Planes Architecture* | 5.1_drm_planes__d1__en.mmd | inserted | Inserted Mermaid block(s) inline after anchor. |
| bai_5_2_hinh_1 | Bai_5.2_Panfrost__raw.md | *Hình 1: Mali Job Manager Flow* | 5.2_mali_architecture__d1__en.mmd | inserted | Inserted Mermaid block(s) inline after anchor. |
| bai_5_3_hinh_1 | Bai_5.3_Compositor__raw.md | *Hình 1: IVI Shell Layer Management* | 5.3_ivi_layers__d1__en.mmd | inserted | Inserted Mermaid block(s) inline after anchor. |
| bai_6_1_hinh_1 | Bai_6.1_VPU_V4L2__raw.md | *Hình 1: Quy trình xử lý Media Request atomic từ Userspace xuống Kernel* | 6.1_media_request_flow__d1__en.mmd | inserted | Inserted Mermaid block(s) inline after anchor. |
| bai_6_2_hinh_1 | Bai_6.2_ZeroCopy__raw.md | *Hình 1: Pipeline không copy dữ liệu giữa VPU và Display* | 6.2_zerocopy_pipeline__d1__en.mmd | inserted | Inserted Mermaid block(s) inline after anchor. |
| bai_6_2_hinh_2 | Bai_6.2_ZeroCopy__raw.md | *Hình 2: Luồng dữ liệu nén AFBC trong SoC* | 6.2_afbc_pipeline__d1__en.mmd | inserted | Inserted Mermaid block(s) inline after anchor. |
| bai_6_2_hinh_3 | Bai_6.2_ZeroCopy__raw.md | *Hình 3: Cơ chế dma_fence giúp CPU không bao giờ bị block* | 6.2_fence_sync__d1__en.mmd | inserted | Inserted Mermaid block(s) inline after anchor. |
| bai_6_3_hinh_1 | Bai_6.3_IntegrationTest__raw.md | *Hình 1: Chiến lược tách luồng và tối ưu hóa hàng đợi* | 6.3_video_optimization__d1__en.mmd | inserted | Inserted Mermaid block(s) inline after anchor. |
| bai_7_1_hinh_1 | Bai_7.1_PreemptRT__raw.md | *Hình 1: Sự khác biệt trong xử lý ngắt và critical section* | 7.1_preempt_rt_comparison__d1__en.mmd | inserted | Inserted Mermaid block(s) inline after anchor. |
| bai_7_3_hinh_1 | Bai_7.3_RT_Workload__raw.md | *Hình 1: Cơ chế Priority Inheritance giải quyết vấn đề* | 7.3_priority_inversion_solution__d1__en.mmd | inserted | Inserted Mermaid block(s) inline after anchor. |

## Unmatched Mermaid Files

- `8.1_ota_ab_partitioning__d1__en.mmd`
- `8.2_dm_verity_structure__d1__en.mmd`
- `8.3_ci_cd_board_farm__d1__en.mmd`
- `9.1_amp_memory__d1__en.mmd`
- `9.2_rtos_boot_flow__d1__en.mmd`
- `Week1_cpu_isolation__d1__en.mmd`
- `Week1_drm_kms_objects__d1__en.mmd`
- `Week1_graphics_stack__d1__en.mmd`
- `Week1_gstreamer_pipeline__d1__en.mmd`
- `Week1_ipc_flow__d1__en.mmd`
- `Week1_ivi_architecture__d1__en.mmd`
- `Week1_rt_task_loop__d1__en.mmd`
- `Week1_sd_layout__d1__en.mmd`
- `Week1_security_layers__d1__en.mmd`
- `Week1_v4l2_decode_flow__d1__en.mmd`
- `Week1_wayland_flow__d1__en.mmd`

## Markdown Files With No Mapped Mermaid

- `Bai_1.3_BootOptimization__raw.md`: no figure captions found
- `Bai_7.2_Isolation__raw.md`: Hình 1: Phân chia workload trên vi xử lý Quad-core

## Uncovered Figure Captions

- `Bai_7.1_PreemptRT__raw.md`: `Hình 2: Các thành phần của độ trễ đánh thức (Wakeup Latency)`

## Anchor Failures

- none
