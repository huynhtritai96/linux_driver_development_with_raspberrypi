# SYSTEM ARCHITECT ARM CORTEX A

# Bài 5.1 - 5.3: DRM/KMS, Panfrost/Mali, IVI-Shell

**Biên soạn:** Phạm Văn Vũ  
**Đơn vị:** HALA Academy  
**Chủ đề:** Advanced Graphics Stack for Embedded ARM Cortex-A Systems

> Ghi chú chuyển đổi: Toàn bộ nội dung PDF đã được chuyển sang Markdown. Các hình ảnh/diagram đã được chuyển thành sơ đồ dạng text để có thể đọc, copy, sửa và không phụ thuộc vào Mermaid renderer.

---

## Mục lục

- [SYSTEM ARCHITECT ARM CORTEX A](#system-architect-arm-cortex-a)
- [Bài 5.1 - 5.3: DRM/KMS, Panfrost/Mali, IVI-Shell](#bài-51---53-drmkms-panfrostmali-ivi-shell)
  - [Mục lục](#mục-lục)
- [Bài 5.1: Advanced DRM/KMS Architecture \& Atomic API](#bài-51-advanced-drmkms-architecture--atomic-api)
  - [Mục tiêu bài học](#mục-tiêu-bài-học)
  - [Phần 1: DRM Planes \& Hardware Composition](#phần-1-drm-planes--hardware-composition)
    - [Hình 1: Hardware Planes Architecture](#hình-1-hardware-planes-architecture)
    - [1.1 Các loại Plane](#11-các-loại-plane)
    - [1.2 Lợi ích của Planes](#12-lợi-ích-của-planes)
  - [Phần 2: Atomic Modesetting API](#phần-2-atomic-modesetting-api)
    - [2.1 Atomic Workflow](#21-atomic-workflow)
    - [Atomic API - ý nghĩa từng bước](#atomic-api---ý-nghĩa-từng-bước)
  - [Phần 3: Advanced Debugging](#phần-3-advanced-debugging)
    - [3.1 EDID \& Modeline](#31-edid--modeline)
    - [3.2 Debugfs Planes State](#32-debugfs-planes-state)
  - [Câu hỏi ôn tập](#câu-hỏi-ôn-tập)
- [Bài 5.2: Advanced Panfrost \& Mali Architecture](#bài-52-advanced-panfrost--mali-architecture)
  - [Mục tiêu bài học](#mục-tiêu-bài-học-1)
  - [Phần 1: Mali Bifrost Internals](#phần-1-mali-bifrost-internals)
    - [Hình 1: Mali Job Manager Flow](#hình-1-mali-job-manager-flow)
    - [1.1 Job Chain của Panfrost](#11-job-chain-của-panfrost)
    - [Data path tổng quát](#data-path-tổng-quát)
  - [Phần 2: GPU Frequency Scaling - Devfreq](#phần-2-gpu-frequency-scaling---devfreq)
    - [2.1 Tuning Governor](#21-tuning-governor)
    - [Governor behavior](#governor-behavior)
  - [Phần 3: Benchmark - Wayland vs X11](#phần-3-benchmark---wayland-vs-x11)
    - [3.1 Benchmark Table](#31-benchmark-table)
    - [3.2 Chạy test](#32-chạy-test)
  - [Phần 4: Troubleshooting Panfrost Performance](#phần-4-troubleshooting-panfrost-performance)
    - [4.1 GPU Hangs](#41-gpu-hangs)
    - [4.2 Memory Bandwidth Warning](#42-memory-bandwidth-warning)
  - [Câu hỏi ôn tập](#câu-hỏi-ôn-tập-1)
- [Bài 5.3: IVI-Shell Architecture \& Jank Analysis](#bài-53-ivi-shell-architecture--jank-analysis)
  - [Mục tiêu bài học](#mục-tiêu-bài-học-2)
  - [Phần 1: IVI-Shell Deep Dive](#phần-1-ivi-shell-deep-dive)
    - [Hình 1: IVI Shell Layer Management](#hình-1-ivi-shell-layer-management)
    - [1.1 Layer Management Protocol](#11-layer-management-protocol)
    - [Layer / Surface mapping example](#layer--surface-mapping-example)
  - [Phần 2: Direct Scanout - Optimization](#phần-2-direct-scanout---optimization)
    - [2.1 Cơ chế](#21-cơ-chế)
    - [2.2 Cấu hình Weston](#22-cấu-hình-weston)
    - [Direct Scanout decision flow](#direct-scanout-decision-flow)
  - [Phần 3: Advanced Jank Analysis](#phần-3-advanced-jank-analysis)
    - [3.1 Weston Debug Timeline](#31-weston-debug-timeline)
    - [3.2 Perfetto Tracing](#32-perfetto-tracing)
    - [What to inspect](#what-to-inspect)
    - [Jank analysis flow](#jank-analysis-flow)
  - [Câu hỏi ôn tập](#câu-hỏi-ôn-tập-2)
- [Tổng kết nhanh](#tổng-kết-nhanh)
  - [Graphics stack overview](#graphics-stack-overview)
  - [Khi tối ưu graphics trên embedded Linux](#khi-tối-ưu-graphics-trên-embedded-linux)
- [Tài liệu tham khảo](#tài-liệu-tham-khảo)
- [Yêu cầu bài tập gợi ý](#yêu-cầu-bài-tập-gợi-ý)

---

# Bài 5.1: Advanced DRM/KMS Architecture & Atomic API

## Mục tiêu bài học

Sau bài học này, học viên sẽ có khả năng:

- Chuyển dịch từ **Legacy API** sang **Atomic Modesetting API**.
- Tối ưu hóa hiển thị với **DRM Planes** và **Hardware Composition**.
- Xử lý **EDID** và **Custom Modeline** nâng cao.

---

## Phần 1: DRM Planes & Hardware Composition

**DRM Planes** cho phép GPU giảm tải việc composite bằng cách sử dụng các hardware overlay chuyên dụng của **Display Engine (DE)**.

### Hình 1: Hardware Planes Architecture

```text
DRM Planes & Hardware Composition

+----------------------------------------------------------------------------------------+
| Display Engine (DE)                                                                    |
|   +-------------------------+     +-------------------------+     +----------------+   |
|   | Primary Plane           |     | Cursor Plane            |     | Overlay Plane  |   |
|   | Z-Order: 0              |     | Z-Order: 2              |     | Z-Order: 1     |   |
|   | +---------------------+ |     | +---------------------+ |     | +--------------+   |
|   | | Framebuffer 1       | |     | | Framebuffer 3       | |     | | FrameBuffer 2|   |
|   | | UI/Desktop          | |     | | Mouse Icon          | |     | | Video YUV    |   |
|   | +---------------------+ |     | +---------------------+ |     | +--------------+   |
|   +-----------+-------------+     +------------+------------+     +------------+-------+ 
|               | alpha blend                    | alpha blend       alpha blend  |      |
|               v                                v                                v      |
|                         +-----------------------------------+                          |
|                         | CRTC / Blender                    |                          |
|                         +-----------------+-----------------+                          |
|                                           |                                            |
+-------------------------------------------+--------------------------------------------+
                                            | Pixel Stream
                                            v
                                  +-------------------+
                                  | Output            |
                                  | +---------------+ |
                                  | | Encoder HDMI  | |
                                  | +-------+-------+ |
                                  |         | HDMI Signal 
                                  |         v         |
                                  | +---------------+ |
                                  | | Monitor       | |
                                  | +---------------+ |
                                  +-------------------+

Overlay Plane hardware note:
- Hardware scaling
- YUV -> RGB conversion
- Zero-copy video path
```

### 1.1 Các loại Plane

| Loại | Chức năng | Mô tả |
|---|---|---|
| Primary | Base Layer | Thường là framebuffer chính, luôn được hiển thị. |
| Overlay | Video/Sprite | Chứa video YUV hoặc UI popup, hỗ trợ scaling/format riêng. |
| Cursor | Mouse Pointer | Kích thước nhỏ, ví dụ 64x64, di chuyển nhanh mà không cần redraw toàn bộ màn hình. |

### 1.2 Lợi ích của Planes

- **Zero-Copy Video:** Video frame, thường là `dma-buf`, được đẩy thẳng vào Overlay Plane. GPU không cần tham gia composite.
- **Power Saving:** Giảm băng thông memory và giảm GPU load.

---

## Phần 2: Atomic Modesetting API

**Atomic Modesetting API** khắc phục nhược điểm của Legacy API như xé hình khi update nhiều thuộc tính và thiếu tính transactional.

### 2.1 Atomic Workflow

```c
// 1. Allocate Request
drmModeAtomicReq *req = drmModeAtomicAlloc();

// 2. Add Properties (Transactional)
drmModeAtomicAddProperty(req, plane_id, "FB_ID", fb_id);
drmModeAtomicAddProperty(req, plane_id, "CRTC_ID", crtc_id);
drmModeAtomicAddProperty(req, crtc_id, "MODE_ID", blob_id);
drmModeAtomicAddProperty(req, crtc_id, "ACTIVE", 1);

// 3. Commit (Test Only - Verify)
ret = drmModeAtomicCommit(fd, req, DRM_MODE_ATOMIC_TEST_ONLY, NULL);

// 4. Commit (Real)
ret = drmModeAtomicCommit(fd, req, DRM_MODE_PAGE_FLIP_EVENT, NULL);
```

### Atomic API - ý nghĩa từng bước

| Bước | API | Ý nghĩa |
|---|---|---|
| 1 | `drmModeAtomicAlloc()` | Tạo atomic request. |
| 2 | `drmModeAtomicAddProperty()` | Gom nhiều thay đổi vào một transaction. |
| 3 | `DRM_MODE_ATOMIC_TEST_ONLY` | Kiểm tra cấu hình có hợp lệ không, chưa apply thật. |
| 4 | `DRM_MODE_PAGE_FLIP_EVENT` | Commit thật và nhận page-flip event. |

---

## Phần 3: Advanced Debugging

### 3.1 EDID & Modeline

Khi màn hình không nhận đúng độ phân giải, cần parse EDID hoặc ép modeline.

```bash
# Dump EDID raw data
cat /sys/class/drm/card0-HDMI-A-1/edid > edid.bin

# Parse EDID, cần cài edid-decode
edid-decode edid.bin

# Add custom modeline bằng kernel cmdline
video=HDMI-A-1:1920x1080@60e
```

Ý nghĩa ví dụ `video=`:

```text
video=HDMI-A-1:1920x1080@60e
      |        |         |  |
      |        |         |  +-- force enable
      |        |         +----- refresh rate 60 Hz
      |        +--------------- resolution 1920x1080
      +------------------------ DRM connector name
```

### 3.2 Debugfs Planes State

```bash
# Inspect plane state: Z-order, coords, fb, crtc
cat /sys/kernel/debug/dri/0/state
```

Example output:

```text
plane[30]: Overlay-1
    crtc=crtc-0
    fb=35
    src=0,0,1920,1080
    dst=100,100,400,300    <-- Windowed Video
```

---

## Câu hỏi ôn tập

1. Hardware Overlay giúp tiết kiệm năng lượng như thế nào?
2. Tại sao Atomic API lại ưu việt hơn Legacy `SetCrtc` / `SetPlane`?
3. Lệnh `video=` trong kernel cmdline dùng để làm gì?

---

# Bài 5.2: Advanced Panfrost & Mali Architecture

## Mục tiêu bài học

Sau bài học này, học viên sẽ có khả năng:

- Deep dive vào kiến trúc **Mali Bifrost (G31)**.
- Tối ưu hóa **Job Manager** và **Devfreq governor**.
- Benchmark và so sánh **Native Wayland** với **X11/Glamor**.

---

## Phần 1: Mali Bifrost Internals

**Mali G31** sử dụng kiến trúc **Bifrost**, tối ưu cho hiệu năng/watt với unified shader cores.

### Hình 1: Mali Job Manager Flow

![alt text](<1. PDFs/Diagrams/5.2_mali_architecture.png>)

### 1.1 Job Chain của Panfrost

1. **Userspace (Mesa)** tạo command buffers.
2. Submit `ioctl()` xuống kernel driver `panfrost.ko`.
3. Kernel driver map job vào **Job Slots** của GPU, ví dụ Vertex/Tiler/Fragment.
4. GPU thực thi job và bắn IRQ khi hoàn thành.

### Data path tổng quát

```text
Mesa Command Buffer
        |
        | ioctl(SUBMIT)
        v
Panfrost Kernel Driver
        |
        | map memory + configure MMU
        v
GPU Job Slots
        |
        | schedule fragment / vertex / compute
        v
Shader Cores
        |
        v
IRQ completion back to kernel
```

---

## Phần 2: GPU Frequency Scaling - Devfreq

GPU cần điều chỉnh xung nhịp động để cân bằng nhiệt và hiệu năng.

### 2.1 Tuning Governor

```bash
# Kiểm tra governor hiện tại
cat /sys/class/devfreq/1800000.gpu/governor
# simple_ondemand

# Chuyển sang Performance: max frequency luôn luôn
echo performance > /sys/class/devfreq/1800000.gpu/governor

# Tinh chỉnh simple_ondemand
echo 50 > /sys/class/devfreq/1800000.gpu/load_threshold
# Giảm ngưỡng load để GPU tăng xung sớm hơn
```

### Governor behavior

| Governor | Ý nghĩa | Khi dùng |
|---|---|---|
| `simple_ondemand` | Tăng/giảm xung theo GPU load. | Cân bằng power/performance. |
| `performance` | Giữ xung cao nhất. | Benchmark hoặc workload cần latency thấp. |
| `powersave` | Giữ xung thấp. | Tiết kiệm năng lượng. |

---

## Phần 3: Benchmark - Wayland vs X11

Native Wayland thường nhanh hơn do loại bỏ một lớp copy và overhead từ X Server.

### 3.1 Benchmark Table

| Benchmark | Native Wayland | X11 / Glamor | Mô tả |
|---|---:|---:|---|
| `glmark2-es2` | 450 FPS | 380 FPS | Wayland loại bỏ overhead của X11 protocol. |
| Latency | 16 ms | 25 ms | Wayland direct scanout giảm trễ. |
| Memory | Low | High | X Server chiếm thêm memory cho pixmaps. |

### 3.2 Chạy test

```bash
# Native Wayland
glmark2-es2-wayland --fullscreen

# XWayland, giả lập X11 trên Weston
# Cần cài XWayland và set DISPLAY=:0
glmark2-es2
```

---

## Phần 4: Troubleshooting Panfrost Performance

### 4.1 GPU Hangs

```bash
dmesg | grep "gpu sched timeout"
```

Nguyên nhân có thể:

- Shader quá phức tạp thực thi quá lâu.
- Memory corruption.
- Job dependency hoặc synchronization issue.

### 4.2 Memory Bandwidth Warning

Triệu chứng:

- Màn hình chớp nháy khi nhiều layer hoạt động.
- Có thể do hết băng thông DRAM.

Giải pháp:

- Dùng texture nén, ví dụ ASTC hoặc ETC2.
- Giảm độ phân giải render target.
- Giảm số layer phải composite bằng GPU.
- Ưu tiên direct scanout hoặc hardware overlay nếu có thể.

---

## Câu hỏi ôn tập

1. Devfreq governor `simple_ondemand` hoạt động như thế nào?
2. Tại sao Native Wayland lại có FPS cao hơn X11 trong hầu hết các trường hợp?
3. Dấu hiệu nhận biết GPU đang bị thắt cổ chai là gì?

---

# Bài 5.3: IVI-Shell Architecture & Jank Analysis

## Mục tiêu bài học

Sau bài học này, học viên sẽ có khả năng:

- Làm chủ kiến trúc **IVI-Shell** và **Layer Management**.
- Hiểu kỹ thuật tối ưu hóa **Direct Scanout**.
- Phân tích **Jank** / giật / lag với **Perfetto**.

---

## Phần 1: IVI-Shell Deep Dive

**IVI-Shell** được thiết kế riêng cho ô tô, nơi các ứng dụng được quản lý chặt chẽ theo ID và layer thay vì cửa sổ tự do như desktop.

### Hình 1: IVI Shell Layer Management

![alt text](<1. PDFs/Diagrams/5.3_ivi_layers.png>)

```text
Control arrows:
- Create / Destroy: IVI controller creates or destroys surfaces/layers.
- Set Visibility: IVI controller controls which surfaces/layers are visible.
- Configure: IVI controller configures position, size, stacking order, etc.

Layer notes:
- Layer 3000: High priority, always on top.
- Layer 2000: Main application area, managed by lifecycle.
- Layer 1000: Background layer.
```

### 1.1 Layer Management Protocol

- **Surface ID:** Mỗi ứng dụng IVI được gán một ID duy nhất. Ví dụ:
  - Navigation = `10`
  - Media = `20`
- **Layer ID:** Layout màn hình được chia thành các lớp. Ví dụ:
  - Background = `1000`
  - App = `2000`
  - Popup = `3000`
- **Control:** `ivi-controller` map Surface vào Layer thông qua ID.

### Layer / Surface mapping example

| Layer ID | Layer name | Surface ID | Surface name | Priority / role |
|---:|---|---:|---|---|
| 3000 | Warning / Popup | 30 | Battery Low | Highest priority, always on top. |
| 2000 | Applications | 10 | Navigation | Active application. |
| 2000 | Applications | 20 | Media Player | Inactive application. |
| 1000 | Background | 1 | Wallpaper | Background. |

---

## Phần 2: Direct Scanout - Optimization

**Direct Scanout** giúp bỏ qua bước composition của GPU khi chỉ có một ứng dụng fullscreen đang chạy.

### 2.1 Cơ chế

```text
Normal path:

App Buffer
    |
    v
GPU Composite
    |
    v
Framebuffer
    |
    v
Display

Direct Scanout path:

App Buffer -----------------------------------------------> Display
```

### 2.2 Cấu hình Weston

Weston tự động kích hoạt Direct Scanout nếu đủ điều kiện:

- Surface là fullscreen.
- Không có layer nào khác đè lên.
- Surface là opaque.
- Buffer format được Display Controller hỗ trợ.

```bash
# Kiểm tra log để xem Direct Scanout có hoạt động không
grep "assigning plane" /tmp/weston.log
```

### Direct Scanout decision flow

```text
New frame from application
        |
        v
Is surface fullscreen?
        |
        +-- no  --> GPU composition
        |
        +-- yes
              |
              v
       Is surface opaque and unobstructed?
              |
              +-- no  --> GPU composition
              |
              +-- yes
                    |
                    v
          Is buffer format supported by Display Controller?
                    |
                    +-- no  --> GPU composition
                    |
                    +-- yes --> Assign hardware plane / Direct Scanout
```

---

## Phần 3: Advanced Jank Analysis

Jank là hiện tượng frame bị trễ, drop hoặc render không đều, gây cảm giác giật/lag.

### 3.1 Weston Debug Timeline

```bash
# Kích hoạt debug timeline
weston --debug

# Sử dụng weston-debug tool
weston-debug -l timeline > weston_timeline.json

# Load JSON vào chrome://tracing hoặc Perfetto UI
```

### 3.2 Perfetto Tracing

Perfetto là công cụ mạnh để trace toàn bộ hệ thống, bao gồm kernel và userspace.

```bash
# 1. Start Perfetto record
perfetto -c config.pbtx -o trace.protobuf

# 2. Run IVI scenario, ví dụ video playback + navigation

# 3. Stop and analyze
# Upload trace.protobuf lên ui.perfetto.dev
```

### What to inspect

Tìm kiếm các đoạn xử lý kéo dài quá 16 ms trên hệ thống 60 FPS, ví dụ:

```text
60 FPS budget:
1 second / 60 frames = 16.67 ms per frame

If one frame takes 20 ms:
- It misses the 16.67 ms deadline.
- Display may repeat the previous frame.
- User sees jank / stutter / dropped frame.
```

Các event cần chú ý:

- `vkms_atomic_commit` kéo dài quá 16 ms.
- `panfrost_job_submit` kéo dài quá 16 ms.
- GPU composition quá lâu.
- Blocking I/O trên render thread.
- Surface/layer update quá nhiều trong cùng một frame.

### Jank analysis flow

```text
User observes jank
        |
        v
Capture Weston timeline + Perfetto trace
        |
        v
Find frame intervals > 16.67 ms
        |
        v
Classify delay source
        |
        +--> GPU bound
        |       - panfrost job too long
        |       - too many layers/composition
        |       - texture bandwidth issue
        |
        +--> Display/KMS bound
        |       - atomic commit too slow
        |       - plane assignment failed
        |       - fallback to GPU composition
        |
        +--> CPU/userspace bound
        |       - application render thread blocked
        |       - IPC delay
        |       - scheduler latency
        |
        v
Apply optimization
        |
        +--> enable direct scanout
        +--> reduce layers
        +--> use hardware overlay
        +--> tune GPU devfreq
        +--> reduce render target resolution
        |
        v
Re-capture trace and compare
```

---

## Câu hỏi ôn tập

1. Quy tắc ID trong IVI-Shell giúp ích gì cho việc quản lý ứng dụng?
2. Direct Scanout cải thiện hiệu năng và độ trễ như thế nào?
3. Nếu một frame mất 20 ms để render, điều gì sẽ xảy ra trên màn hình 60 Hz?

---

# Tổng kết nhanh

## Graphics stack overview

```text
Application / IVI App
        |
        | Wayland / IVI protocol
        v
Weston / IVI-Shell
        |
        | DRM Atomic API
        v
DRM/KMS Kernel Driver
        |
        +--> Planes
        +--> CRTC / Blender
        +--> Encoder / Connector
        |
        v
Display Engine / HDMI / Panel

GPU path:
Application -> Mesa -> Panfrost -> Mali G31 -> dma-buf/framebuffer -> DRM/KMS
```

## Khi tối ưu graphics trên embedded Linux

| Vấn đề | Công cụ kiểm tra | Hướng tối ưu |
|---|---|---|
| Plane không hoạt động | `/sys/kernel/debug/dri/0/state` | Kiểm tra format, size, z-order, atomic test. |
| Sai độ phân giải | EDID, `edid-decode`, `video=` | Parse EDID hoặc force modeline. |
| GPU chậm | `dmesg`, devfreq sysfs, benchmark | Tune governor, giảm render target, giảm layer. |
| Jank | Weston timeline, Perfetto | Tìm frame > 16.67 ms, classify bottleneck. |
| X11 chậm hơn | Benchmark Wayland vs X11 | Ưu tiên Native Wayland nếu có thể. |

---

# Tài liệu tham khảo

- HALA Academy - System Architect ARM Cortex A - Bài 5.1: Advanced DRM/KMS Architecture & Atomic API
- HALA Academy - System Architect ARM Cortex A - Bài 5.2: Advanced Panfrost & Mali Architecture
- HALA Academy - System Architect ARM Cortex A - Bài 5.3: IVI-Shell Architecture & Jank Analysis

---

# Yêu cầu bài tập gợi ý

- Kiểm tra DRM plane state bằng `cat /sys/kernel/debug/dri/0/state`.
- Thử force mode bằng kernel cmdline `video=HDMI-A-1:1920x1080@60e`.
- Benchmark GPU bằng `glmark2-es2-wayland --fullscreen`.
- Kiểm tra Panfrost lỗi bằng `dmesg | grep "gpu sched timeout"`.
- Bật Weston debug timeline và phân tích frame timing bằng Perfetto.