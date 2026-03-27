# Bài 6.3: Video System Optimization & Profiling

## Page 1

Bài 6.3: Advanced Video System Optimization & Profiling

# Biên soạn: Phạm Văn Vũ

## Page 2

### Mục tiêu Bài học

```text
      • Áp dụng các kỹ thuật Multithreading để tối ưu hóa GStreamer pipeline.
      • Sử dụng eBPF (Extended Berkeley Packet Filter) để đo độ trễ phần cứng với độ chính xác nano
       giây.
      • Thực hiện Integration Test toàn diện và đánh giá chất lượng video tự động (Quality
       Assessment).
```

1. Pipeline Optimization Strategies

*Hình 1: Chiến lược tách luồng và tối ưu hóa hàng đợi*
<!-- mermaid-insert:start:bai_6_3_hinh_1 -->
```mermaid
flowchart LR
    subgraph cpu["CPU Domain"]
        src["File Source"]
        demux["Demuxer"]
        parser2["Parser Thread 2"]
        parser["Parser Thread"]
        mt["Multithreading<br/>Offload CPU 0"]
        src --> demux
        demux -->|Stream B| parser2
        src -->|Stream A| parser
        parser2 --> parser
        parser -.-> mt
    end

    subgraph hw["Hardware Domain"]
        vpu["VPU Decoder"]
        display["Display Engine"]
        acc["Hardware Accel<br/>eBPF Monitored"]
    end

    parser -->|Slice Data| vpu
    vpu -->|Zero-Copy (DMA-BUF)| display
    vpu -.-> acc
```
<!-- mermaid-insert:end:bai_6_3_hinh_1 -->

### 1.1 Multithreaded Parsing (Queue Injection)

Một pipeline video điển hình có 3 giai đoạn chính: IO (Đọc file/Mạng), Parsing (Tách NAL Units), và Decoding (Gửi xuống VPU). Mặc định, GStreamer có thể chạy chúng trên cùng 1 thread, dẫn đến hiện tượng nghẽn cổ chai nếu một task tốn nhiều CPU.

Giải pháp: Chèn element `queue` để tạo ranh giới thread (thread boundary).

## Page 3

```text
    # Basic pipeline (Single threaded parsing & decoding setup)
    filesrc ! h264parse ! v4l2h264dec ! ...
```

```text
    # Optimized pipeline
    filesrc ! queue name=io_q ! h264parse ! queue name=dec_q ! v4l2h264dec ! ...
```

```text
      • `io_q`: Tách việc đọc đĩa ra khỏi việc parse.
      • `dec_q`: Tách việc parse ra khỏi việc gọi ioctl xuống VPU.
```

### 1.2 Buffer Pool Tuning

Số lượng buffer trong pool ảnh hưởng trực tiếp đến độ mượt mà. Quá ít buffer gây ra hiện tượng decoder phải chờ display trả buffer về (starvation).

```text
    # Trong Userspace Code
    req.count = 4; // Mặc định
    # Tăng lên 8 hoặc 16 để có thêm "headroom" cho Jitter
    req.count = 12;
    ioctl(fd, VIDIOC_REQBUFS, &req);
```

2. Advanced Profiling với eBPF/BCC

### 2.1 Tại sao dùng eBPF?

Các công cụ như `top` hay `perf` chỉ cho thấy CPU usage trung bình. Để đo chính xác VPU mất bao nhiêu micro-seconds (µs) để decode 1 frame, ta cần trace ngay tại mức driver interrupt handler mà không cần recompile kernel.

### 2.2 Viết script bpftrace đo VPU Latency

Script dưới đây hook vào 2 kernel function của Cedrus driver: lúc bắt đầu chạy (`device_run`) và lúc ngắt xảy ra (`irq_handler`).

```text
    /* cedrus_latency.bt */
    #include
```

```text
    // Hook vào hàm start decoding
    kprobe:cedrus_device_run {
        @start[tid] = nsecs;
    }
```

// Hook vào hàm xử lý ngắt (kết thúc decode)

## Page 4

```text
    kprobe:cedrus_irq {
        $s = @start[tid];
        if ($s != 0) {
            $delta = (nsecs - $s) / 1000; // Đổi sang us
            printf("VPU Decode Time: %d us\n", $delta);
            delete(@start[tid]);
```

```text
                // Vẽ biểu đồ histogram
                @latency_hist = hist($delta);
         }
    }
```

Chạy script trên board:

```text
    bpftrace cedrus_latency.bt
    # Output:
    # VPU Decode Time: 2500 us
    # VPU Decode Time: 2600 us
    # ...
```

3. Automated Video Quality Assessment

### 3.1 PSNR & SSIM

Khi tối ưu hệ thống (ví dụ giảm bitrate, dùng AFBC), ta cần đảm bảo chất lượng hình ảnh không bị suy giảm nghiêm trọng. Hai chỉ số vàng là PSNR (Peak Signal-to-Noise Ratio) và SSIM (Structural Similarity).

### 3.2 Thực hành đo PSNR với FFmpeg

```text
    # So sánh file decode hardware (hw.yuv) với file gốc tham chiếu (ref.yuv)
    ffmpeg -s 1920x1080 -pix_fmt nv12 -i hw.yuv \
           -s 1920x1080 -pix_fmt yuv420p -i ref.yuv \
           -lavfi "psnr" -f null -
```

Ngưỡng chấp nhận: PSNR > 40dB là chất lượng rất tốt (gần như không phân biệt được bằng mắt thường). Nếu PSNR < 30dB, hình ảnh bị vỡ (artifacts) rõ rệt.

## Page 5

4. Tổng kết Tuần 6

Chúng ta đã hoàn thiện module Video & Multimedia với các kiến thức từ kiến trúc driver (V4L2 Request API), tối ưu luồng dữ liệu (Zero-Copy, AFBC) đến kỹ thuật đo đạc và gỡ lỗi chuyên sâu (eBPF). Đây là nền tảng để xây dựng các sản phẩm IVI (In-Vehicle Infotainment) cao cấp, nơi video performance là yếu tố then chốt.

HALA Academy | Biên soạn: Phạm Văn Vũ
