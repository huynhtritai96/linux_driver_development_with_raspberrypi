# SYSTEM ARCHITECT ARM CORTEX A

# Bài 4.1 - 4.3: Boot Profiling, Memory CMA, Scheduler/Perf/Cgroups

**Biên soạn:** Phạm Văn Vũ  
**Đơn vị:** HALA Academy  
**Chủ đề:** Advanced Linux boot-time profiling, DMA-BUF/CMA memory analysis, scheduler/perf/cgroups tuning  
**Dạng chuyển đổi:** Toàn bộ nội dung PDF sang Markdown, tất cả hình ảnh được chuyển thành sơ đồ text để tránh lỗi Mermaid parser.

---

## Mục lục

- [SYSTEM ARCHITECT ARM CORTEX A](#system-architect-arm-cortex-a)
- [Bài 4.1 - 4.3: Boot Profiling, Memory CMA, Scheduler/Perf/Cgroups](#bài-41---43-boot-profiling-memory-cma-schedulerperfcgroups)
  - [Mục lục](#mục-lục)
- [Bài 4.1: Advanced Boot Time Profiling](#bài-41-advanced-boot-time-profiling)
  - [Mục tiêu bài học](#mục-tiêu-bài-học)
  - [Phần 1: Bootstage - U-Boot Profiling](#phần-1-bootstage---u-boot-profiling)
    - [Hình 1: U-Boot Bootstage Timeline](#hình-1-u-boot-bootstage-timeline)
    - [1.1 Cấu hình U-Boot](#11-cấu-hình-u-boot)
    - [1.2 Phân tích Report](#12-phân-tích-report)
  - [Phần 2: Kernel Bootgraph](#phần-2-kernel-bootgraph)
    - [Hình 2: Quy trình Phân tích và Tối ưu Boot Time](#hình-2-quy-trình-phân-tích-và-tối-ưu-boot-time)
    - [2.1 Thu thập dữ liệu](#21-thu-thập-dữ-liệu)
    - [2.2 Phân tích biểu đồ](#22-phân-tích-biểu-đồ)
  - [Phần 3: Grabserial - I/O Latency](#phần-3-grabserial---io-latency)
    - [3.1 Cài đặt \& Sử dụng](#31-cài-đặt--sử-dụng)
    - [3.2 Phân tích Log](#32-phân-tích-log)
  - [Phần 4: initcall\_debug Nâng cao](#phần-4-initcall_debug-nâng-cao)
    - [4.1 Script phân tích tự động](#41-script-phân-tích-tự-động)
    - [4.2 Blacklist Driver/Module](#42-blacklist-drivermodule)
  - [Phần 5: systemd-analyze Deep Dive](#phần-5-systemd-analyze-deep-dive)
    - [5.1 Critical Chain](#51-critical-chain)
    - [5.2 Service Optimization](#52-service-optimization)
  - [Câu hỏi Ôn tập](#câu-hỏi-ôn-tập)
- [Bài 4.2: DMA-BUF Heaps \& Memory Analysis](#bài-42-dma-buf-heaps--memory-analysis)
  - [Mục tiêu bài học](#mục-tiêu-bài-học-1)
  - [Phần 1: DMA-BUF Heaps](#phần-1-dma-buf-heaps)
    - [Hình 1: DMA-BUF Heaps System vs CMA](#hình-1-dma-buf-heaps-system-vs-cma)
    - [1.1 Architecture](#11-architecture)
    - [1.2 Sử dụng - Userspace](#12-sử-dụng---userspace)
  - [Phần 2: Slab Allocator Analysis](#phần-2-slab-allocator-analysis)
    - [2.1 Slabtop Tool](#21-slabtop-tool)
    - [2.2 Debugging Slab Leaks](#22-debugging-slab-leaks)
  - [Phần 3: Memory Fragmentation Detection](#phần-3-memory-fragmentation-detection)
    - [Hình 2: Kernel/Movable Pages \& CMA Region](#hình-2-kernelmovable-pages--cma-region)
    - [3.1 Buddy Info](#31-buddy-info)
    - [3.2 Vmstat Monitor](#32-vmstat-monitor)
  - [Phần 4: Debug dma-buf Orphans](#phần-4-debug-dma-buf-orphans)
  - [Câu hỏi Ôn tập](#câu-hỏi-ôn-tập-1)
- [Bài 4.3: Advanced Scheduler, Perf \& Cgroups](#bài-43-advanced-scheduler-perf--cgroups)
  - [Mục tiêu bài học](#mục-tiêu-bài-học-2)
  - [Phần 1: Perf Sched Analysis](#phần-1-perf-sched-analysis)
    - [Hình 1: Scheduling Context Switch \& Latency Analysis](#hình-1-scheduling-context-switch--latency-analysis)
    - [1.1 Record Scheduling Events](#11-record-scheduling-events)
    - [1.2 Latency Analysis](#12-latency-analysis)
    - [1.3 Visual Map](#13-visual-map)
  - [Phần 2: Cgroups v2 - Resource Control](#phần-2-cgroups-v2---resource-control)
    - [2.1 Setup Cgroups v2](#21-setup-cgroups-v2)
    - [2.2 Limit CPU \& Memory](#22-limit-cpu--memory)
  - [Phần 3: Sysctl Tuning Advanced](#phần-3-sysctl-tuning-advanced)
    - [3.1 Scheduler Migration Cost](#31-scheduler-migration-cost)
    - [3.2 Virtual Memory dirty ratio](#32-virtual-memory-dirty-ratio)
  - [Phần 4: RT Throttling](#phần-4-rt-throttling)
    - [Hình 2: Các Kernel Preemption Models](#hình-2-các-kernel-preemption-models)
  - [Câu hỏi Ôn tập](#câu-hỏi-ôn-tập-2)
- [Appendix: Checklist thực hành nhanh](#appendix-checklist-thực-hành-nhanh)
  - [Boot time profiling checklist](#boot-time-profiling-checklist)
  - [Memory analysis checklist](#memory-analysis-checklist)
  - [Scheduler/perf/cgroups checklist](#schedulerperfcgroups-checklist)
- [Appendix: Tóm tắt lệnh quan trọng](#appendix-tóm-tắt-lệnh-quan-trọng)
  - [U-Boot / Boot profiling](#u-boot--boot-profiling)
  - [Memory / DMA-BUF / Slab / Fragmentation](#memory--dma-buf--slab--fragmentation)
  - [Scheduler / Perf / Cgroups / Sysctl](#scheduler--perf--cgroups--sysctl)

---

# Bài 4.1: Advanced Boot Time Profiling

## Mục tiêu bài học

Sau bài học này, học viên sẽ có khả năng:

- Deep dive vào các công cụ profiling: **Bootstage**, **Bootgraph**, **Grabserial**.
- Phân tích chi tiết **initcall latency** và quá trình **kernel initialization**.
- Thực hành tối ưu hóa thời gian khởi động nâng cao.

---

## Phần 1: Bootstage - U-Boot Profiling

**Bootstage** là công cụ built-in của **U-Boot** để đo thời gian chính xác từng giai đoạn khởi động.

### Hình 1: U-Boot Bootstage Timeline

```text
U-Boot Bootstage Timeline

+----------------+        +---------------------------+
| Time (us)      |        | Boot Stage                |
+----------------+        +---------------------------+
        |                              |
        |            0 us              |
        | Power On / Reset             |
        | ---------------------------> |
        |                              |
        |         125,000 us           |
        | SPL: board_init_f            |  Basic Init (SRAM)
        | ---------------------------> |  - init early board state
        |                              |  - run in SRAM
        |                              |
        |         250,000 us           |
        | SPL: board_init_r            |  DRAM Init
        | ---------------------------> |  - init DRAM controller
        |                              |  - prepare RAM for U-Boot proper
        |                              |
        |         850,000 us           |
        | U-Boot: main_loop            |  Load Environment
        | ---------------------------> |  - load env
        |                              |  - prepare boot command
        |                              |
        |        1,200,000 us          |
        | bootm_start                  |  Load Kernel Image & DTB
        | ---------------------------> |  - load Image
        |                              |  - load device tree blob
        |                              |
        |        2,100,000 us          |
        | kernel_loaded                |  Jump to Linux Kernel
        | ---------------------------> |  - hand off to kernel entry
        |
+----------------+        +---------------------------+
```

### 1.1 Cấu hình U-Boot

```text
CONFIG_BOOTSTAGE=y
CONFIG_BOOTSTAGE_REPORT=y
CONFIG_BOOTSTAGE_USER_COUNT=20
CONFIG_SHOW_BOOT_PROGRESS=y
```

### 1.2 Phân tích Report

Chạy lệnh trong U-Boot:

```bash
=> bootstage report
```

Ví dụ output:

```text
Timer summary in microseconds (12 records):

Mark        Elapsed       Stage
0           0             reset
125,000     125,000       board_init_f
250,000     125,000       board_init_r
850,000     600,000       main_loop
1,200,000   350,000       bootm_start
2,100,000   900,000       kernel_loaded
```

Bảng phân tích:

| Mark | Elapsed | Stage | Ý nghĩa |
|---:|---:|---|---|
| `0` | `0` | `reset` | Mốc reset ban đầu |
| `125,000` | `125,000` | `board_init_f` | Basic init trong SRAM |
| `250,000` | `125,000` | `board_init_r` | DRAM init |
| `850,000` | `600,000` | `main_loop` | Load environment / chuẩn bị bootcmd |
| `1,200,000` | `350,000` | `bootm_start` | Load kernel image và DTB |
| `2,100,000` | `900,000` | `kernel_loaded` | Kernel đã load xong, chuẩn bị jump |

**Lưu ý:** `Elapsed` cho biết thời gian tiêu tốn của từng step. Khi tối ưu boot time, cần tập trung vào các step có elapsed time lớn bất thường.

---

## Phần 2: Kernel Bootgraph

**Bootgraph** giúp trực quan hóa quá trình khởi động kernel dưới dạng biểu đồ timeline.

### Hình 2: Quy trình Phân tích và Tối ưu Boot Time

```text
Boot Time Analysis Flow
                         START
                           v
+------------------------------------------------+
| Measurement                                    |
|   +----------------------------------------+   |
|   | Add initcall_debug to bootargs         |   |
|   +----------------------------------------+   |
|                           v                    |
|   +----------------------------------------+   |
|   | Boot and capture dmesg                 |   |
|   +----------------------------------------+   |
|                           v                    |
|   +----------------------------------------+   |
|   | Run systemd-analyze                    |   |
|   +----------------------------------------+   |
+------------------------------------------------+
                           v
+------------------------------------------------+
| Analysis                                       |
|   +----------------------------------------+   |
|   | Parse slow initcalls                   |   |
|   +----------------------------------------+   |
|                           v                    |
|   +----------------------------------------+   |
|   | Identify critical path                 |   |
|   +----------------------------------------+   |
|                           v                    |
|   +----------------------------------------+   |
|   | Find optimization targets              |   |
|   +----------------------------------------+   |
+------------------------------------------------+
                           v
+--------------------------------------------------------------------------------+
| Optimization                                                                   |
|   +----------------+   +----------------------+   +------------------------+   |
|   | Async probe    |   | Disable slow drivers |   | Defer non-critical work|   |
|   +----------------+   +----------------------+   +------------------------+   |
|          +--------------------+-----------------------+                        |
+--------------------------------------------------------------------------------+
                           |
                           v
+------------------------------------------------+
| Verification                                   |
|   +----------------------------------------+   |
|   | Re-measure boot time                   |   |
|   +----------------------------------------+   |
|                          v                     |
|           +---------------------+              |
|           | Target met?         |              |
|           +----------+----------+              |
|              yes          | no                 |
|               |           |                    |
|               v           v                    |
|   +----------------+   +-------------------+   |
|   | Document       |   | Further analysis  |---+--- loop back
|   | results        |   |                   |       to Analysis
|   +----------------+   +-------------------+   |
|               |                                |
|               v                                |
|              DONE                              |
+------------------------------------------------+
```

### 2.1 Thu thập dữ liệu

```bash
# 1. Thêm vào bootargs
setenv bootargs "initcall_debug printk.time=1 ..."

# 2. Sau khi boot, lấy log dmesg
dmesg > boot.log

# 3. Tạo biểu đồ trên Host PC
scripts/bootgraph.pl boot.log > boot.svg
```

### 2.2 Phân tích biểu đồ

Khi đọc bootgraph, cần tập trung vào:

- Tìm các thanh dài nhất: đó là các initcall hoặc driver có thời gian thực thi lâu nhất.
- Phát hiện các khoảng trống: có thể là CPU idle hoặc đang chờ I/O.
- Xác định thứ tự thực thi của driver: driver chạy tuần tự hay có thể parallel hóa.

---

## Phần 3: Grabserial - I/O Latency

**Grabserial** đo thời gian thực thi thực tế từ góc nhìn bên ngoài qua cổng console. Nó bao gồm cả thời gian firmware/ROM mà kernel không thấy.

### 3.1 Cài đặt & Sử dụng

```bash
# Trên Host PC
pip install grabserial

# Capture boot log với timestamp chính xác
grabserial -d /dev/ttyUSB0 -b 115200 -t -e 30 > boot_timing.log
```

### 3.2 Phân tích Log

Ví dụ log:

```text
[0.000000] TF-A BL31
[0.150000] U-Boot SPL
[0.850000] Loading Kernel...    <-- Bottleneck tại load kernel?
[2.100000] Starting kernel...
```

Công thức tính thời gian giữa hai mốc:

```text
Delta = Timestamp[n] - Timestamp[n-1]
```

Ví dụ:

| Đoạn | Tính toán | Ý nghĩa |
|---|---:|---|
| TF-A -> U-Boot SPL | `0.150000 - 0.000000 = 0.15s` | Firmware/SPL early boot |
| U-Boot SPL -> Loading Kernel | `0.850000 - 0.150000 = 0.70s` | U-Boot/SPL preparation |
| Loading Kernel -> Starting kernel | `2.100000 - 0.850000 = 1.25s` | Load kernel/DTB/initramfs |

---

## Phần 4: initcall_debug Nâng cao

### 4.1 Script phân tích tự động

```bash
#!/bin/bash
# analyze_initcalls.sh

dmesg | grep "initcall" | sed 's/\(.*\) returned.*after \(.*\) usecs/\2 \1/' | \
awk '{ if ($1 > 1000) print $0 }' | sort -nr | head -20
```

Mục tiêu của script:

- Lấy log `initcall` từ `dmesg`.
- Trích thời gian thực thi theo đơn vị `usecs`.
- Chỉ in ra các initcall chậm hơn `1000 us`.
- Sắp xếp giảm dần để thấy top bottleneck.

### 4.2 Blacklist Driver/Module

Nếu driver không cần thiết gây chậm boot, có thể disable hoặc blacklist.

Cách 1: Thêm vào bootargs:

```text
initcall_blacklist=driver_name_init
```

Cách 2: Tắt trong kernel menuconfig:

```text
Device Drivers -> ... -> [ ] Enable Driver X
```

---

## Phần 5: systemd-analyze Deep Dive

### 5.1 Critical Chain

Lệnh:

```bash
systemd-analyze critical-chain
```

Ví dụ:

```text
graphical.target @2.5s
└─multi-user.target @2.4s
  └─network.target @2.1s
    └─NetworkManager.service @1.8s +300ms   <-- Critical path!
      └─...
```

Ý nghĩa:

- `@2.5s`: thời điểm target/service được active.
- `+300ms`: thời gian service tiêu tốn để start.
- Critical path là chuỗi dependency quyết định thời điểm boot hoàn tất.

### 5.2 Service Optimization

Một số hướng tối ưu service:

| Hướng tối ưu | Ý nghĩa |
|---|---|
| `Type=notify` -> `Type=simple` | Chuyển sang `simple` nếu không cần chờ service báo start-up complete |
| `Before/After` | Giảm dependencies không cần thiết |
| `TimeoutSec` | Giảm timeout mặc định, ví dụ từ `90s` xuống thấp hơn |

---

## Câu hỏi Ôn tập

1. **Bootstage khác gì với `bootchart`?**
2. **Tại sao `grabserial` lại quan trọng khi đo thời gian BIOS/Firmware?**
3. **Làm thế nào để tìm ra driver nào chiếm nhiều thời gian nhất khi boot kernel?**

---

# Bài 4.2: DMA-BUF Heaps & Memory Analysis

## Mục tiêu bài học

Sau bài học này, học viên sẽ có khả năng:

- Nắm vững cơ chế **DMA-BUF Heaps** thay thế **ION**.
- Phân tích chi tiết **Slab allocator** với `slabtop`.
- Debug **Memory Leak** và **Fragmentation** với `vmstat`.

---

## Phần 1: DMA-BUF Heaps

**DMA-BUF Heaps** là giao diện userspace hiện đại để cấp phát contiguous memory, thay thế cho ION framework cũ.

### Hình 1: DMA-BUF Heaps System vs CMA

```text
DMA-BUF Heaps Architecture

+-------------------- Userspace --------------------+
|                                                   |
|   +-------------------------------------------+   |
|   | Application (Camera / Display)            |   |
|   +-------------------------------------------+   |
|             | /dev/dma_heap/system    | /dev/dma_heap/reserved
+-------------|-------------------------|-----------+
              |                         |
              v                         v
+--------------- Kernel DMA-BUF Heaps ------------------------+
|  +----------------------------+   +----------------------+  |
|  | System Heap                |   | CMA Heap             |  |
|  | +----------------------+   |   | +----------------+   |  |
|  | | alloc_pages()        |   |   | | dma_alloc_     |   |  |
|  | +----------------------+   |   | | contiguous()   |   |  |
|  |                            |   | +----------------+   |  |
|  | Non-contiguous             |   | Physically           |  |
|  | - vmalloc                  |   | contiguous           |  |
|  | - cacheable/uncacheable    |   | for legacy HW        |  |
|  +-------------|--------------+   +----------|-----------+  |
|                |                             |              |
+----------------|-----------------------------|--------------+
                 |                     OK      |
                 | OK via IOMMU        +-------| Required
                 v                     v       v
+-------------------------------------------------------------+
| I/O Hardware                                                |
|  +----------------------------+   +----------------------+  |
|  | MMU-Capable                |   | Simple DMA           |  |
|  | GPU / ISP                  |   | Display              |  |
|  +----------------------------+   +----------------------+  |
+-------------------------------------------------------------+
```

### 1.1 Architecture

| Heap | Cách cấp phát | Phù hợp với |
|---|---|---|
| **System Heap** | Cấp phát trang rời rạc, thường qua `alloc_pages()` hoặc `vmalloc` | CPU/GPU có MMU/IOMMU |
| **CMA Heap** | Cấp phát vùng nhớ vật lý liên tục | Display/Video HW cũ hoặc DMA engine cần contiguous physical memory |

### 1.2 Sử dụng - Userspace

```c
int heap_fd = open("/dev/dma_heap/system", O_RDONLY);

struct dma_heap_allocation_data data = {
    .len = 4 * 1024 * 1024, // 4MB
    .fd_flags = O_RDWR | O_CLOEXEC,
    .heap_flags = 0,
};

ioctl(heap_fd, DMA_HEAP_IOCTL_ALLOC, &data);

// data.fd is now a dma-buf fd
```

Luồng cấp phát:

```text
Application
    |
    | open("/dev/dma_heap/system")
    v
DMA-BUF Heap device
    |
    | ioctl(DMA_HEAP_IOCTL_ALLOC)
    v
Kernel allocator
    |
    | returns data.fd
    v
Userspace receives dma-buf fd
```

---

## Phần 2: Slab Allocator Analysis

**Slab allocator** quản lý các kernel object nhỏ như `inode`, `task_struct`, `dentry` để tránh fragmentation.

### 2.1 Slabtop Tool

```bash
# Hiển thị TOP kernel memory consumers
slabtop -s c
```

Ví dụ output:

```text
OBJS    ACTIVE  USE   OBJ SIZE  CACHE SIZE  NAME
50000   49000   98%   0.19K     9700K       dentry
10000   9000    90%   1.00K     10000K      kmalloc-1k
```

Cách đọc nhanh:

| Cột | Ý nghĩa |
|---|---|
| `OBJS` | Tổng số object trong cache |
| `ACTIVE` | Số object đang được sử dụng |
| `USE` | Tỷ lệ active/tổng |
| `OBJ SIZE` | Kích thước mỗi object |
| `CACHE SIZE` | Tổng bộ nhớ cache sử dụng |
| `NAME` | Tên slab cache |

**Dấu hiệu leak:** Nếu `dentry` hoặc `inode_cache` tăng liên tục không giảm, đây có thể là dấu hiệu Directory Cache không được thu hồi.

### 2.2 Debugging Slab Leaks

Bật kernel config:

```text
CONFIG_SLUB_DEBUG=y
CONFIG_SLUB_DEBUG_ON=y
```

Kiểm tra allocator log:

```bash
cat /proc/slab_allocators
```

---

## Phần 3: Memory Fragmentation Detection

Sự phân mảnh bộ nhớ làm giảm hiệu suất và có thể ngăn cản việc cấp phát các khối bộ nhớ lớn liên tục.

### Hình 2: Kernel/Movable Pages & CMA Region

```text
CMA Memory Architecture

+----------------------------------------------------------+
| Allocators                                               |
|  +----------------------+   +-------------------------+  |
|  | dma_alloc_coherent() |   | alloc_pages(GFP_MOVABLE)|  |
|  +----------|-----------+   +----------|--------------+  |
+-------------|--------------------------|-----------------+
              | Allocate contiguous      | Use when CMA idle
              v                          v
+------------------------------------------------------------------------------------------------------------------------+
| Physical Memory (DRAM) - 1GB to 4GB                                                                                    |
|  +----------------------------------------------+                                                                      |
|  | CMA Region (256MB)                           |                                                                      |
|  | +------------------+  +--------------------+ |  +-------------------------+     +-------------------------------+   |
|  | | Allocated        |  | Free               | |  | Kernel Pages            |     | Movable Pages                 |   |
|  | | DMA buffers      |  | reusable by        | |  | (vmalloc, kmalloc)      |     | (User pages, can migrate)     |   |
|  | |                  |  | movable pages      | |  +-------------------------+     +-------------------------------+   |
|  | +------------------+  +--------------------+ |                                                                      |
|  +----------------------------------------------+                                                                      |
+------------------------------------------------------------------------------------------------------------------------+
                              |
                              v
+--------------------------------------------------------------------------+
| CMA - Contiguous Memory Allocator                                        |
|                                                                          |
| - Reserved at boot time                                                  |
| - Used by VPU, GPU, Camera                                               |
| - Movable pages can use when free                                        |
+--------------------------------------------------------------------------+
```

### 3.1 Buddy Info

```bash
cat /proc/buddyinfo
```

Ví dụ:

```text
Node 0, zone Normal 100 50 30 10 5 2 1 0 0 0 0
#                   ^   ^  ^  ^  ^ ^ ^ ^ ^ ^ ^
#                   4K  8K 16K ...         4MB
```

Cách đọc:

| Vị trí cột | Block size ví dụ | Ý nghĩa |
|---|---|---|
| Cột trái | 4K, 8K, 16K... | Block nhỏ |
| Cột phải | 1MB, 2MB, 4MB... | Block lớn |

**Dấu hiệu high external fragmentation:** các cột bên phải, tức các block lớn, đều là `0`. Khi đó CMA allocation có thể thất bại.

### 3.2 Vmstat Monitor

```bash
# Monitor memory events, delay 1s
vmstat 1
```

Các cột cần chú ý:

| Cột | Ý nghĩa | Dấu hiệu vấn đề |
|---|---|---|
| `si` | Swap In | Khác `0` nghĩa là hệ thống thiếu RAM hoặc đang swap |
| `so` | Swap Out | Khác `0` nghĩa là hệ thống thiếu RAM hoặc đang swap |
| `buff/cache` | Disk cache | Cache quá lớn có thể gây áp lực memory |
| `free` | Free RAM | Free quá thấp cần xem thêm reclaim/swap |

---

## Phần 4: Debug dma-buf Orphans

Khi ứng dụng crash mà không close `dma-buf fd`, memory có thể bị leak.

Mount debugfs:

```bash
mount -t debugfs none /sys/kernel/debug
```

List active dma-bufs:

```bash
cat /sys/kernel/debug/dma_buf/bufinfo
```

Ví dụ output:

```text
size      flags   mode   count   exp_name
4194304   000000  0600   2       panfrost-gem
```

Cách đọc:

| Cột | Ý nghĩa |
|---|---|
| `size` | Kích thước buffer |
| `flags` | Flags của buffer |
| `mode` | Permission mode |
| `count` | Reference count |
| `exp_name` | Exporter name |

**Dấu hiệu leak:** Kiểm tra cột `count`. Nếu `count` cao bất thường mà không có process nào dùng, có thể có orphan/leak.

---

## Câu hỏi Ôn tập

1. **ION framework đã bị thay thế bởi công nghệ nào?**
2. **Làm thế nào để phát hiện kernel object leak?**
3. **Dấu hiệu nhận biết external fragmentation trong `/proc/buddyinfo` là gì?**

---

# Bài 4.3: Advanced Scheduler, Perf & Cgroups

## Mục tiêu bài học

Sau bài học này, học viên sẽ có khả năng:

- Phân tích latency sâu với **`perf sched`**.
- Quản lý tài nguyên hệ thống với **Cgroups v2**.
- System tuning nâng cao với **sysctl**.

---

## Phần 1: Perf Sched Analysis

**Perf Sched** ghi lại mọi sự kiện scheduling như `switch` và `wakeup` để phân tích độ trễ.

### Hình 1: Scheduling Context Switch & Latency Analysis

```text
Perf Sched Latency Analysis

Lanes:
+--------------+-------------------+----------------------+----------------+
| CPU Core     | Task A (YouTube)  | Task B (Background)  | Scheduler      |
+--------------+-------------------+----------------------+----------------+

Normal Execution
----------------
CPU Core             Task A                Task B                 Scheduler
   |                   |                     |                       |
   |--- Running 5ms -->|                     |                       |
   |                   |--- Preempted: time slice expired ---------->|
   |                   |                     |<--- Switch Context ---|
   |                   |                     |                       |
   |----------------- Running -------------->|                       |
   |                   |                     |                       |

Latency Event
-------------
CPU Core             Task A                Task B                 Scheduler
   |                   |                     |                       |
   |                   |                     |------- Sleep/Yield -->|
   |                   |<------------------- Wakeup! ----------------|
   |                   |                     |                       |
   |                   |                     |                       | Target: Switch to A
   |                   |                     |                       |Timestamp: T1 
   |                   |                     |                       |
   |<---------------- Spinlock Contention? --------------------------|
   |                   |                     |                       |
   |  Delay: 7ms (Latency!)                  |                       |
   |                   |                     |                       |
   |---- Running ----->|                     |                       |
   |                   |                     |                       |
   |                   |  Timestamp: T2      |                       |
   |                   |  Latency = T2 - T1  |                       |
```

### 1.1 Record Scheduling Events

```bash
# Record 5 seconds of scheduling
perf sched record -- sleep 5
```

### 1.2 Latency Analysis

```bash
perf sched latency --sort max
```

Ví dụ output:

```text
Task       Runtime     Switches   Average Delay   Maximum Delay
youtube    25.321 ms   150        0.012 ms        7.321 ms *
kworker    10.111 ms   400        0.005 ms        0.123 ms
```

Phân tích:

| Task | Dấu hiệu |
|---|---|
| `youtube` | Max delay lên tới `7ms`, có thể gây dropped frames |
| `kworker` | Delay nhỏ hơn nhiều, ít nghiêm trọng hơn |

### 1.3 Visual Map

```bash
perf sched map
```

Mục đích:

- Hiển thị biểu đồ ASCII mô tả task switching trên từng CPU.
- Giúp quan sát task nào chạy, task nào bị preempt, task nào wakeup.

---

## Phần 2: Cgroups v2 - Resource Control

**Cgroups v2** dùng để kiểm soát tối đa CPU/Memory mà một nhóm process được phép sử dụng.

### 2.1 Setup Cgroups v2

```bash
# Mount
mount -t cgroup2 none /sys/fs/cgroup

# Create group
mkdir /sys/fs/cgroup/limit_g
```

### 2.2 Limit CPU & Memory

Giới hạn CPU tối đa 50%:

```bash
# CPU Limit: Max 50% CPU
# quota 50000us / period 100000us
echo "50000 100000" > /sys/fs/cgroup/limit_g/cpu.max
```

Giới hạn memory tối đa 128MB:

```bash
# Memory Limit: Max 128MB
echo 134217728 > /sys/fs/cgroup/limit_g/memory.max
```

Add process vào cgroup:

```bash
echo $PID > /sys/fs/cgroup/limit_g/cgroup.procs
```

Luồng sử dụng:

```text
Process PID
    |
    | echo $PID > cgroup.procs
    v
/sys/fs/cgroup/limit_g
    |
    +-- cpu.max     = "50000 100000"  -> max 50% CPU
    |
    +-- memory.max  = "134217728"     -> max 128MB
```

---

## Phần 3: Sysctl Tuning Advanced

Sysctl tuning dùng để tinh chỉnh các tham số kernel sâu hơn để tối ưu throughput hoặc latency.

### 3.1 Scheduler Migration Cost

```bash
# Chi phí ước tính, đơn vị ns, để migrate task giữa các core
cat /proc/sys/kernel/sched_migration_cost_ns

# Default: 500000 (0.5ms)
```

Ý nghĩa tuning:

| Hướng chỉnh | Tác động |
|---|---|
| Tăng `sched_migration_cost_ns` | Giữ task ở lại CPU lâu hơn, tốt hơn cho cache locality và throughput |
| Giảm `sched_migration_cost_ns` | Load balance tích cực hơn, có thể tốt hơn cho responsiveness |

### 3.2 Virtual Memory dirty ratio

```bash
# Khi nào bắt đầu ghi data xuống đĩa?

# Bắt đầu writeback khi 10% RAM bẩn, absolute limit
sysctl -w vm.dirty_ratio=10

# Background writeback khi 5% RAM bẩn
sysctl -w vm.dirty_background_ratio=5
```

Giảm dirty ratio giúp hệ thống mượt hơn, tránh I/O stall lớn khi flush cache.

---

## Phần 4: RT Throttling

**RT Throttling** là cơ chế bảo vệ hệ thống khỏi các RT process bị treo hoặc chiếm 100% CPU.

### Hình 2: Các Kernel Preemption Models

```text
Linux Preemption Models

+--------------------------+   +---------------------------+   +---------------------------+   +---------------------------+
| PREEMPT_NONE             |   | PREEMPT_VOLUNTARY         |   | PREEMPT                   |   | PREEMPT_RT                |
| Server                   |   | Desktop                   |   | Low-Latency               |   | Real-Time                 |
+--------------------------+   +---------------------------+   +---------------------------+   +---------------------------+
| - No kernel preemption   |   | - Preempt at yield points |   | - Preempt anywhere        |   | - Full preemption         |
| - Max throughput         |   | - Good throughput         |   | - Reduced throughput      |   | - IRQ threading           |
| - High latency (ms)      |   | - Medium latency          |   | - Low latency, sub-ms     |   | - Deterministic, us       |
| - Use: servers, batch    |   | - Use: desktop            |   | - Use: audio/video        |   | - Use: industrial RT      |
+--------------------------+   +---------------------------+   +---------------------------+   +---------------------------+

                                                                                  Latency:    NONE > VOLUNTARY > PREEMPT > RT
                                                                                  Throughput: NONE > VOLUNTARY > PREEMPT > RT
```

Mặc định, RT tasks chỉ được chạy 95% thời gian, tức khoảng `0.95s` trong mỗi `1s`.

```bash
cat /proc/sys/kernel/sched_rt_period_us
# 1000000 (1s)

cat /proc/sys/kernel/sched_rt_runtime_us
# 950000 (0.95s)
```

Nếu muốn cho RT task dùng 100% CPU:

```bash
# Nguy hiểm: có thể làm system không còn CPU cho non-RT tasks
sysctl -w kernel.sched_rt_runtime_us=-1
```

**Cảnh báo:** Set `sched_rt_runtime_us = -1` có thể làm hệ thống bị starve nếu RT task bị lỗi hoặc loop vô hạn.

---

## Câu hỏi Ôn tập

1. **`perf sched latency` cung cấp thông tin quan trọng gì?**
2. **Cgroups v2 khác gì v1 về cấu trúc thư mục?**
3. **Tại sao nên giảm `vm.dirty_ratio` trên thiết bị nhúng?**

---

# Appendix: Checklist thực hành nhanh

## Boot time profiling checklist

```text
[ ] Enable U-Boot Bootstage
[ ] Run bootstage report
[ ] Add initcall_debug printk.time=1 to bootargs
[ ] Capture dmesg to boot.log
[ ] Generate bootgraph SVG
[ ] Capture external serial timing with grabserial
[ ] Find slow initcalls
[ ] Find systemd critical-chain
[ ] Optimize one target at a time
[ ] Re-measure and document result
```

## Memory analysis checklist

```text
[ ] Check dma-buf heaps under /dev/dma_heap/
[ ] Allocate test buffer from system heap
[ ] Allocate test buffer from CMA/reserved heap if available
[ ] Check slabtop -s c
[ ] Monitor dentry/inode_cache growth
[ ] Check /proc/buddyinfo for large block availability
[ ] Monitor vmstat 1 for swap/cache/free behavior
[ ] Mount debugfs
[ ] Check /sys/kernel/debug/dma_buf/bufinfo
[ ] Look for abnormal dma-buf reference count
```

## Scheduler/perf/cgroups checklist

```text
[ ] Run perf sched record -- sleep 5
[ ] Run perf sched latency --sort max
[ ] Run perf sched map
[ ] Mount cgroup2
[ ] Create cgroup limit_g
[ ] Set cpu.max
[ ] Set memory.max
[ ] Add PID to cgroup.procs
[ ] Inspect sched_migration_cost_ns
[ ] Tune dirty_ratio and dirty_background_ratio carefully
[ ] Inspect RT throttling parameters
```

---

# Appendix: Tóm tắt lệnh quan trọng

## U-Boot / Boot profiling

```bash
# U-Boot
bootstage report

# Linux bootargs
setenv bootargs "initcall_debug printk.time=1 ..."

# Kernel log
dmesg > boot.log

# Bootgraph
scripts/bootgraph.pl boot.log > boot.svg

# Grabserial
grabserial -d /dev/ttyUSB0 -b 115200 -t -e 30 > boot_timing.log

# Slow initcalls
dmesg | grep "initcall" | sed 's/\(.*\) returned.*after \(.*\) usecs/\2 \1/' | \
awk '{ if ($1 > 1000) print $0 }' | sort -nr | head -20

# systemd critical path
systemd-analyze critical-chain
```

## Memory / DMA-BUF / Slab / Fragmentation

```bash
# DMA-BUF heap devices
ls /dev/dma_heap/

# Slab memory consumers
slabtop -s c

# Slab allocators
cat /proc/slab_allocators

# Buddy allocator state
cat /proc/buddyinfo

# Memory monitor
vmstat 1

# DebugFS
mount -t debugfs none /sys/kernel/debug
cat /sys/kernel/debug/dma_buf/bufinfo
```

## Scheduler / Perf / Cgroups / Sysctl

```bash
# perf sched
perf sched record -- sleep 5
perf sched latency --sort max
perf sched map

# cgroups v2
mount -t cgroup2 none /sys/fs/cgroup
mkdir /sys/fs/cgroup/limit_g
echo "50000 100000" > /sys/fs/cgroup/limit_g/cpu.max
echo 134217728 > /sys/fs/cgroup/limit_g/memory.max
echo $PID > /sys/fs/cgroup/limit_g/cgroup.procs

# scheduler migration cost
cat /proc/sys/kernel/sched_migration_cost_ns

# dirty ratio tuning
sysctl -w vm.dirty_ratio=10
sysctl -w vm.dirty_background_ratio=5

# RT throttling
cat /proc/sys/kernel/sched_rt_period_us
cat /proc/sys/kernel/sched_rt_runtime_us
```