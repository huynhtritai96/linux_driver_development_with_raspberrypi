# Bài 1.1: Trusted Firmware-A (TF-A)

## Page 1

# Bài 1.1: Trusted Firmware-A (TF-A)

# Biên soạn: Phạm Văn Vũ

## Page 2

### Mục tiêu Bài học

Sau buổi học này, học viên sẽ có khả năng:

- Hiểu rõ vai trò của EL3 (Exception Level 3) trong kiến trúc ARM64

- Nắm vững cấu trúc và chức năng của Trusted Firmware-A (TF-A)

- Thực hành build TF-A cho Orange Pi Zero 3 (Allwinner H618)

## Page 3

### Phần 1: Lý thuyết

### 1.1 ARM Exception Levels

## Page 4

_No extractable text found on page 4._

## Page 5

*Hình 1: Quy trình Boot Chain*
<!-- mermaid-insert:start:bai_1_1_hinh_1 -->
```mermaid
sequenceDiagram
    participant PWR as Power Supply
    participant BROM as ROM Code (BROM)
    participant BOOT0 as Boot0 (eGON)
    participant TFA as TF-A BL31
    participant SPL as U-Boot SPL
    participant UBOOT as U-Boot Proper
    participant LINUX as Linux Kernel
    PWR->>BROM: Power good
    Note right of BROM: 32KB internal ROM<br/>Reads NAND/eMMC/SD<br/>Entry: 0xFFFF0000
    BROM->>BROM: Check eFUSE
    BROM->>BROM: Determine boot media
    BROM->>BOOT0: Load to SRAM (0x20000)
    Note right of BOOT0: Allwinner first-stage loader<br/>Size: 32KB max<br/>Location: 8KB offset on SD
    BOOT0->>BOOT0: Init PLLs (CPU, DDR)
    BOOT0->>BOOT0: Configure DDR PHY
    BOOT0->>BOOT0: DDR training
    BOOT0->>BOOT0: Memory test
    Note right of BOOT0: DRAM base: 0x40000000<br/>Size: 1-4GB
    BOOT0->>TFA: Load BL31 to SRAM
    Note right of TFA: Exception Level 3<br/>Secure monitor<br/>PSCI implementation
    TFA->>TFA: Init GIC-400
    TFA->>TFA: Set up secure world
    TFA->>TFA: Register PSCI handlers
    Note right of TFA: CPU_ON: 0xC4000003<br/>CPU_OFF: 0x84000002<br/>SYSTEM_RESET: 0x84000009
    TFA->>SPL: Jump to BL33 (EL3 -> EL2)
    Note right of TFA: TF-A remains resident for SMC handling
    Note right of SPL: Runs in SRAM<br/>Minimal drivers<br/>Size: about 64KB max
    SPL->>SPL: board_init_f()
    SPL->>SPL: serial_init()
    SPL->>SPL: spl_mmc_load_image()
    SPL->>UBOOT: Load to DRAM (0x4A000000)
    Note right of UBOOT: Full featured<br/>Device model<br/>Filesystems
    UBOOT->>UBOOT: board_init_r()
    UBOOT->>UBOOT: Init device model
    UBOOT->>UBOOT: Load environment
    UBOOT->>UBOOT: Run bootcmd
    Note right of UBOOT: kernel_addr_r = 0x40080000<br/>fdt_addr_r = 0x4FA00000<br/>ramdisk_addr_r = 0x4FE00000
    UBOOT->>UBOOT: Load mmc 0:1 Image
    UBOOT->>UBOOT: Load mmc 0:1 DTB
    UBOOT->>LINUX: booti (EL2 -> EL1)
    Note right of LINUX: head.S -> start_kernel()<br/>Set up MMU<br/>Driver probing
    Note right of LINUX: Boot complete in about 2-4 seconds
```
<!-- mermaid-insert:end:bai_1_1_hinh_1 -->

ARM64 định nghĩa 4 cấp độ đặc quyền (Exception Levels):

Level        Tên gọi                      Mục đích                               Ví dụ

EL0          User                         Ứng dụng người dùng                    Apps, Games

EL1          Kernel                       Hệ điều hành                           Linux Kernel

EL2          Hypervisor                   Ảo hóa                                 KVM, Xen

EL3          Secure Monitor               Bảo mật                                TF-A, OP-TEE

Tại sao cần nhiều Exception Levels?

- Cách ly bảo mật: Mỗi level có quyền hạn riêng
- Bảo vệ hệ thống: Code ở level thấp không thể truy cập level cao
- Hỗ trợ ảo hóa: EL2 cho phép chạy nhiều OS cùng lúc

### 1.2 Secure World vs Normal World

Thành phần                 Secure World                               Normal World

EL3                                                      TF-A (Secure Monitor)

EL2                        -                                          Hypervisor (KVM)

EL1                        Secure OS (OP-TEE)                         Linux Kernel

EL0                        Trusted Apps                               User Applications

Giải thích:

- Secure World: Chạy code tin cậy (keys, DRM, payment)
- Normal World: Chạy Linux và ứng dụng thông thường
- TF-A (EL3): Điểm chuyển giao giữa 2 thế giới

## Page 6

### 1.3 Trusted Firmware-A (TF-A)

*Hình 2: Kiến trúc TF-A*
<!-- mermaid-insert:start:bai_1_1_hinh_2 -->
**Boot Loader Stages**
```mermaid
flowchart TB
    bl1["BL1<br/>AP Trusted ROM"]
    bl1_note["Primary bootloader<br/>Not used on Allwinner<br/>Replaced by Boot0"]
    bl2["BL2<br/>Trusted Boot Firmware"]
    bl2_note["Load BL31, BL32, BL33<br/>Not used on Allwinner<br/>Replaced by Boot0"]
    bl31["BL31<br/>EL3 Runtime"]
    bl31_note["Always active<br/>Runs at EL3<br/>Handles SMC calls"]
    bl32["BL32<br/>Secure OS"]
    bl32_note["Optional (OP-TEE)<br/>Runs at S-EL1<br/>Trusted apps"]
    bl33["BL33<br/>Non-Secure"]
    bl33_note["U-Boot<br/>Runs at EL2/EL1<br/>Normal world"]
    bl1 --> bl2
    bl2 --> bl31
    bl2 -. optional secure payload .-> bl32
    bl31 --> bl33
    bl32 -. trusted world alongside non-secure boot .-> bl33
    bl1 -.-> bl1_note
    bl2 -.-> bl2_note
    bl31 -.-> bl31_note
    bl32 -.-> bl32_note
    bl33 -.-> bl33_note
```

**SMC Call Flow**
```mermaid
sequenceDiagram
    participant NW as Normal World (Linux/U-Boot)
    participant EL3 as EL3 Entry
    participant ROUTER as Service Router
    participant PSCI as PSCI Handler
    NW->>EL3: SMC instruction (#0)
    Note over NW,EL3: Trap to EL3
    EL3->>ROUTER: Context switch
    ROUTER->>PSCI: Route by function ID
    Note right of PSCI: CPU_ON: 0xC4000003<br/>CPU_OFF: 0x84000002<br/>SUSPEND: 0xC4000001
    PSCI-->>NW: ERET back
```

**BL31 Internal Architecture**
```mermaid
flowchart TB
    subgraph platform["Platform Layer"]
        h616["sun50i_h616"]
        papi["Platform APIs"]
    end
    subgraph psci_lib["PSCI Library"]
        cpu_pwr["CPU Power Mgmt"]
        sys_pwr["System Power Mgmt"]
        hotplug["CPU Hotplug"]
    end
    subgraph common["Common Libraries"]
        ctx["Context Mgmt"]
        intr["Interrupt Mgmt"]
        exc["Exception Handling"]
    end
    subgraph runtime["Runtime Services"]
        smc["SMC Dispatcher"]
        psci["PSCI Service"]
        sip["SiP Service"]
    end
    subgraph drivers["Drivers"]
        gic["GIC-400 Driver"]
        timer["Timer Driver"]
        uart["UART Driver"]
    end
    h616 --> papi
    papi -->|Platform hooks| cpu_pwr
    papi -->|Platform hooks| sys_pwr
    cpu_pwr -->|Register service| smc
    sys_pwr -->|Register service| psci
    ctx -->|Use libs| smc
    intr -->|Use libs| smc
    exc -->|Use libs| psci
    smc -->|Access HW| gic
    psci -->|Access HW| timer
    sip -->|Access HW| uart
```
<!-- mermaid-insert:end:bai_1_1_hinh_2 -->

Các thành phần Boot Loader (BLx)

Component              Chạy tại                        Chức năng

BL1                    ROM/SRAM                        First stage, validate BL2

BL2                    SRAM                            Load BL31, BL32, BL33

BL31                   SRAM (resident)                 EL3 Runtime, PSCI

BL32                   RAM                             Secure OS (OP-TEE) - Optional

BL33                   DRAM                            Non-secure bootloader (U-Boot)

Lưu ý cho Allwinner H618: Boot0 (Allwinner proprietary) thay thế BL1+BL2, chỉ cần build BL31.

PSCI (Power State Coordination Interface)

PSCI là giao diện chuẩn SMC (Secure Monitor Call) để quản lý nguồn:

Function                              PSCI ID                       Mô tả

CPU_ON                                0xC4000003                    Bật một CPU core

CPU_OFF                               0x84000002                    Tắt CPU hiện tại

## Page 7

CPU_SUSPEND    0xC4000001       Suspend CPU

SYSTEM_RESET   0x84000009       Reset toàn hệ thống

SYSTEM_OFF     0x84000008       Tắt nguồn

## Page 8

### Phần 2: Thực hành (Lab)

### 2.1 Chuẩn bị môi trường

Bước 1: Cài đặt Cross-compiler

```text
    # Ubuntu/Debian
    sudo apt update
    sudo apt install gcc-aarch64-linux-gnu make git
```

```text
    # Verify
    aarch64-linux-gnu-gcc --version
```

Bước 2: Clone TF-A source

```text
    cd ~/opi_build
    git clone https://github.com/ARM-software/arm-trusted-firmware.git
    cd arm-trusted-firmware
```

```text
    # Kiểm tra version
    git describe --tags
```

### 2.2 Build BL31

```text
    # Clean và build
    make CROSS_COMPILE=aarch64-linux-gnu- PLAT=sun50i_h616 DEBUG=1 bl31
```

```text
    # Kiểm tra output
    ls -la build/sun50i_h616/debug/bl31.bin
```

Các tham số build quan trọng

Tham số                              Giá trị            Mô tả

PLAT                                 sun50i_h616        Platform cho H616/H618

DEBUG                                1                  Enable debug output

## Page 9

LOG_LEVEL                                   40                      Verbose logging

SUNXI_PSCI_USE_NATIVE                       1                       Native PSCI

### 2.3 Xác nhận kết quả

Khi boot thành công, UART sẽ hiển thị:

```text
    NOTICE:    BL31: v2.9(release):v2.9.0
    NOTICE:    BL31: Built : 10:30:45, Jan 05 2026
    NOTICE:    BL31: Detected Allwinner H616 SoC
    NOTICE:    BL31: PSCI: System Reset called
```

Các lỗi thường gặp

Lỗi                         Nguyên nhân              Giải pháp

No BL31 output              Sai PLAT                 Dùng sun50i_h616

Hang sau BL31               BL33 path sai            Kiểm tra U-Boot build

PSCI not working            Thiếu config             Enable SUNXI_PSCI_USE_NATIVE

## Page 10

### Phần 3: Câu hỏi Ôn tập

1. ARM64 có bao nhiêu Exception Levels? Liệt kê và mô tả ngắn gọn.

2. TF-A BL31 chạy ở Exception Level nào? Tại sao?

3. PSCI là gì? Nêu 3 function quan trọng nhất.

4. Tại sao với Allwinner H618, chúng ta chỉ cần build BL31?

5. Giải thích sự khác biệt giữa Secure World và Normal World.

### Phần 4: Tài liệu Tham khảo

• TF-A Documentation: https://trustedfirmware-a.readthedocs.io/ • ARM PSCI Specification: https://developer.arm.com/documentation/den0022/ • Allwinner H616 in TF-A: https://github.com/ARM-software/arm-trusted-firmware/tree/master/plat/ allwinner

Yêu cầu Bài tập

- Screenshot bootlog hiển thị "BL31: v2.x"
- File bl31.bin đã build thành công
- Ghi chú các bước đã thực hiện

HALA Academy | Biên soạn: Phạm Văn Vũ
