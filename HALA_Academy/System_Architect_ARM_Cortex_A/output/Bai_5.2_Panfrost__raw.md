# Bài 5.2: Advanced Panfrost & Mali Architecture

## Page 1

# Bài 5.2: Advanced Panfrost & Mali Architecture

# Biên soạn: Phạm Văn Vũ

## Page 2

### Mục tiêu Bài học

```text
      • Deep dive vào kiến trúc Mali Bifrost (G31)
      • Tối ưu hóa Job Manager và Devfreq governor
      • Benchmark và so sánh: Native Wayland vs X11/Glamor
```

### Phần 1: Mali Bifrost Internals

Mali G31 sử dụng kiến trúc Bifrost, tối ưu cho hiệu năng/watt với unified shader cores.

*Hình 1: Mali Job Manager Flow*
<!-- mermaid-insert:start:bai_5_2_hinh_1 -->
```mermaid
flowchart TD
    subgraph userspace["Userspace (Mesa)"]
        cmd["Command Buffer (Job)"]
    end

    subgraph kernel["Kernel (Panfrost)"]
        jm["Job Manager"]
        mmu_cfg["MMU Config"]
    end

    subgraph hw["Mali-G31 GPU Hardware"]
        subgraph slots["Job Slots"]
            frag["Slot 0: Fragment"]
            vertex["Slot 1: Vertex/Tiler"]
            compute["Slot 2: Compute"]
        end
        subgraph shader["Shader Cores"]
            core0["Core 0"]
            core1["Core 1"]
        end
        l2["L2 Cache"]
        mmu["MMU"]
    end

    cmd -->|ioctl(SUBMIT)| jm
    jm -->|Map Memory| mmu_cfg
    jm -->|Schedule Fragment Job| frag
    jm -->|Schedule Vertex Job| vertex
    frag -->|Execute| core0
    vertex -->|Execute| core1
    core0 --> l2
    core1 --> l2
```
<!-- mermaid-insert:end:bai_5_2_hinh_1 -->

### 1.1 Job Chain của Panfrost

1. Userspace (Mesa) tạo command buffers.

## Page 3

```text
     2. Submit ioctl() xuống Kernel Driver (panfrost.ko).
     3. Kernel Driver map job vào "Job Slots" của GPU (Vertex/Tiler/Fragment).
     4. GPU thực thi và bắn IRQ khi hoàn thành.
```

### Phần 2: GPU Frequency Scaling (Devfreq)

GPU cần điều chỉnh xung nhịp động để cân bằng nhiệt và hiệu năng.

### 2.1 Tuning Governor

```text
    # Kiểm tra governor hiện tại
    cat /sys/class/devfreq/1800000.gpu/governor
    # simple_ondemand
```

```text
    # Chuyển sang Performance (Max Frequency luôn luôn)
    echo performance > /sys/class/devfreq/1800000.gpu/governor
```

```text
    # Tinh chỉnh simple_ondemand
    echo 50 > /sys/class/devfreq/1800000.gpu/load_threshold
    # (Giảm ngưỡng load để GPU tăng xung sớm hơn)
```

### Phần 3: Benchmark: Wayland vs X11

Native Wayland thường nhanh hơn do loại bỏ một lớp copy (X Server).

### 3.1 Benchmark Table

Benchmark        Native Wayland       X11 (Glamor)        Mô tả

glmark2-es2      450 FPS              380 FPS             Wayland loại bỏ overhead của X11 protocol.

Latency          16ms                 25ms                Wayland direct scanout giảm trễ.

Memory           Low                  High                X Server chiếm thêm memory cho pixmaps.

## Page 4

### 3.2 Chạy Test

```text
    # Native Wayland
    glmark2-es2-wayland --fullscreen
```

```text
    # XWayland (giả lập X11 trên Weston)
    # Cần cài XWayland và set DISPLAY=:0
    glmark2-es2
```

### Phần 4: Troubleshooting Panfrost Performance

### 4.1 GPU Hangs

dmesg | grep "gpu sched timeout"

Nguyên nhân: Shader quá phức tạp thực thi quá lâu, hoặc memory corruption.

### 4.2 Memory Bandwidth Warning

Màn hình chớp nháy khi nhiều layer hoạt động? Có thể do hết băng thông DRAM.

```text
      • Giải pháp: Dùng texture nén (ASTC/ETC2).
      • Giảm độ phân giải render target.
```

Câu hỏi Ôn tập

```text
     1. Devfreq governor "simple_ondemand" hoạt động như thế nào?
     2. Tại sao Native Wayland lại có FPS cao hơn X11 trong hầu hết các trường hợp?
     3. Dấu hiệu nhận biết GPU đang bị thắt cổ chai (bottleneck)?
```

HALA Academy | Biên soạn: Phạm Văn Vũ
