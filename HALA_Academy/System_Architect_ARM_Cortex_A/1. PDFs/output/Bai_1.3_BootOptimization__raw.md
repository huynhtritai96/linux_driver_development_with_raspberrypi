# Bài 1.3: Tối ưu Boot Time & Fail-safe

## Page 1

# Bài 1.3: Tối ưu Boot Time & Fail-safe

# Biên soạn: Phạm Văn Vũ

## Page 2

### Mục tiêu Bài học

Sau buổi học này, học viên sẽ có khả năng:

- Đo lường và phân tích thời gian boot

- Áp dụng các kỹ thuật tối ưu boot time

- Thiết lập cơ chế fail-safe/recovery

### Phần 1: Đo lường Boot Time

### 1.1 Các giai đoạn Boot và thời gian điển hình

Giai đoạn                                   Thời gian   % Tổng

ROM + Boot0                                 ~100ms      5%

TF-A (BL31)                                 ~50ms       2%

U-Boot SPL                                  ~200ms      10%

U-Boot Proper                               ~1000ms     50%

Kernel + Init                               ~700ms      35%

Tổng                                        ~2s         100%

### 1.2 Công cụ đo lường

grabserial - Ghi log UART với timestamp

```text
    # Cài đặt
    pip install grabserial
```

## Page 3

```text
    # Chạy
    grabserial -d /dev/ttyUSB0 -b 115200 -t -m "login:" -q "Starting kernel"
```

bootchart - Phân tích bootup trong Linux

```text
    # Enable trong kernel
    CONFIG_SCHEDSTATS=y
```

```text
    # Cài đặt tool
    sudo apt install bootchart
```

### Phần 2: Tối ưu U-Boot

### 2.1 Giảm Boot Delay

```text
    # Mặc định: 2 giây chờ user interrupt
    setenv bootdelay 0     # Bỏ hoàn toàn delay
```

```text
    # Hoặc: Chờ key nhưng không in countdown
    setenv bootdelay -2
```

### 2.2 Silent Console

```text
    # Tắt output để giảm overhead UART
    setenv silent 1
```

```text
    # Trong menuconfig
    CONFIG_SILENT_CONSOLE=y
    CONFIG_SILENT_U_BOOT_ONLY=y
```

### 2.3 Falcon Mode (SPL Boot trực tiếp)

SPL có thể boot trực tiếp Kernel mà không qua U-Boot Proper:

```text
    # Trong menuconfig
    CONFIG_SPL_OS_BOOT=y
    CONFIG_SPL_FALCON_BOOT_MMCSD=y
```

## Page 4

Falcon Mode giúp tiết kiệm ~1 giây bằng cách bỏ qua U-Boot Proper.

### 2.4 Loại bỏ features không cần

```text
    # Disable USB nếu không dùng
    CONFIG_USB=n
```

```text
    # Disable Network nếu boot từ SD
    CONFIG_NET=n
```

```text
    # Disable EFI nếu không cần
    CONFIG_EFI_LOADER=n
```

## Page 5

### Phần 3: Fail-safe Boot

### 3.1 Kiến trúc A/B Partition

Partition                    Size             Nội dung

Boot0 + SPL                  8KB              Bootloader stage 1

U-Boot                       2MB              Bootloader stage 2

Env                          64KB             Environment variables

Boot_A                       64MB             Kernel + DTB (slot A)

Root_A                       2GB              Rootfs (slot A)

Boot_B                       64MB             Kernel + DTB (slot B)

Root_B                       2GB              Rootfs (slot B)

### 3.2 Boot Script với Retry

```text
    # boot.scr content
    setenv boot_slot A
    setenv boot_retry 0
```

```text
    # Try Slot A
    if load mmc 0:2 ${kernel_addr_r} Image; then
        echo "Booting Slot A..."
        booti ${kernel_addr_r} - ${fdt_addr_r}
    fi
```

```text
    # Fallback to Slot B
    echo "Slot A failed, trying Slot B..."
    if load mmc 0:4 ${kernel_addr_r} Image; then
        echo "Booting Slot B..."
        booti ${kernel_addr_r} - ${fdt_addr_r}
    fi
```

```text
    # Last resort: Recovery
    echo "Both slots failed! Entering recovery..."
```

## Page 6

```text
    load mmc 0:1 ${kernel_addr_r} recovery.img
    booti ${kernel_addr_r}
```

### 3.3 Watchdog Protection

```text
    # Enable watchdog trong U-Boot
    CONFIG_WDT=y
    CONFIG_WDT_SUNXI=y
```

```text
    # Reset nếu không boot trong 60s
    wdt start 60000
```

### 3.4 Boot Success Marker

```text
    # Trong boot script
    setenv boot_success 0
    saveenv
```

```text
    # Sau khi kernel boot OK (trong init script)
    fw_setenv boot_success 1
```

### Phần 4: Bảng Tổng hợp Optimization

Kỹ thuật                     Tiết kiệm           Độ khó          Risk

bootdelay=0                  ~2s                 Dễ              Thấp

Silent console               ~200ms              Dễ              Thấp

Disable USB/Net              ~500ms              Trung bình      Trung bình

Falcon mode                  ~1s                 Khó             Cao

Custom bootcmd               ~300ms              Trung bình      Trung bình

## Page 7

### Phần 5: Script Đo Boot Time

```text
    #!/bin/bash
    # measure_boot_time.sh
```

```text
    DEVICE=/dev/ttyUSB0
    BAUD=115200
    OUTPUT=bootlog_$(date +%Y%m%d_%H%M%S).txt
```

```text
    echo "Recording boot log to $OUTPUT..."
    echo "Power on the board now. Press Ctrl+C to stop."
```

grabserial -d $DEVICE -b $BAUD -t -o $OUTPUT

```text
    # Parse results
    echo ""
    echo "=== Boot Time Analysis ==="
    grep -E "BL31:|U-Boot|Starting kernel" $OUTPUT
```

### Phần 6: Câu hỏi Ôn tập

1. Giai đoạn nào trong boot process chiếm nhiều thời gian nhất?

2. Liệt kê 3 kỹ thuật tối ưu boot time dễ áp dụng nhất.

3. Falcon mode là gì? Ưu và nhược điểm?

4. Giải thích kiến trúc A/B partition và lợi ích của nó.

5. Watchdog được sử dụng như thế nào trong fail-safe boot?

Tài liệu Tham khảo

- eLinux Boot Time: https://elinux.org/Boot_Time
- U-Boot Falcon Mode: https://docs.u-boot.org/en/latest/develop/falcon.html

## Page 8

- Bootlin Boot Time Optimization: https://bootlin.com/doc/training/boot-time/

Yêu cầu Bài tập

- Bootlog TRƯỚC khi tối ưu (với thời gian)
- Bootlog SAU khi tối ưu (với thời gian)
- Bảng so sánh thời gian từng giai đoạn
- Script recovery hoạt động (demo video/log)

HALA Academy | Biên soạn: Phạm Văn Vũ
