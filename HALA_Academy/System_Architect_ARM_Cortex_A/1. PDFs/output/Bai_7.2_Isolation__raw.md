# Bài 7.2: Advanced CPU Isolation & Housekeeping

## Page 1

```text
               Bài 7.2: Advanced CPU Isolation &
                             Housekeeping
```

# Biên soạn: Phạm Văn Vũ

## Page 2

### Mục tiêu Bài học

```text
      • Thiết lập mô hình "Housekeeping CPUs" chuyên nghiệp cho hệ thống multi-core.
      • Sử dụng Cgroups v2 `cpuset` controller để quản lý tài nguyên.
      • Tối ưu hóa Timer Tick với cấu hình `nohz_full`.
```

1. CPU Housekeeping Strategy

### 1.1 Khái niệm Housekeeping

Trong một hệ thống 4 cores (như H618), không phải core nào cũng chạy Real-time. Chúng ta chia CPU thành 2 nhóm:

```text
      • Housekeeping CPUs (Core 0-1): Chịu trách nhiệm chạy OS services (SSH, Logging, Disk I/O,
       GUI) và nhận ngắt từ phần cứng thông thường.
      • Isolated CPUs (Core 2-3): Được "làm sạch" hoàn toàn. Không nhận ngắt, không chạy background
       tasks, không timer tick. Dành riêng cho RT Loop.
```

```text
                                      CPU Housekeeping Topology
                             Hình 1: Phân chia workload trên vi xử lý Quad-core
```

2. Cấu hình Isolation Chi tiết

### 2.1 Kernel Boot Parameters

Để báo cho Linux Kernel biết về sự phân chia này ngay từ lúc boot, ta thêm vào `bootargs`:

```text
    isolcpus=2,3              # Scheduler không tự ý đẩy task thường vào core 2,3
    nohz_full=2,3             # Tắt timer tick (100Hz/250Hz) trên core 2,3 nếu chỉ có 1
    task chạy
    rcu_nocbs=2,3             # Đẩy việc xử lý RCU callbacks về core 0,1
    irqaffinity=0,1           # Mặc định các IRQ phần cứng chỉ được route vào core 0,1
```

## Page 3

### 2.2 Dịch chuyển IRQ (IRQ Migration)

Mặc dù đã set `irqaffinity`, một số driver vẫn có thể request IRQ trên core 2,3. Ta cần script để ép chúng về core 0,1.

```text
    # Script: force_irq_migration.sh
    for irq in /proc/irq/*; do
         if [ -d "$irq" ]; then
             # Mask 0x3 (Binary 0011) -> CPU 0 và CPU 1
             echo 3 > "$irq/smp_affinity" 2>/dev/null
         fi
    done
```

3. Lab: Scripting the Isolation

Dưới đây là script thực tế để tự động hóa quy trình cô lập CPU và di chuyển IRQ. Lưu file này thành

`setup_isolation.sh` và chạy trên board.

```text
    #!/bin/bash
    # setup_isolation.sh: Automate CPU Isolation for H618
```

```text
    RT_CPUS="2-3"
    NON_RT_CPUS="0-1"
```

echo "=== Starting CPU Isolation Setup ==="

```text
    # 1. Disable IRQ Balance
    if systemctl is-active --quiet irqbalance; then
        echo "[*] Stopping irqbalance..."
        systemctl stop irqbalance
    fi
```

```text
    # 2. Migrate IRQs to Housekeeping CPUs (0-1)
    echo "[*] Migrating IRQs to CPUs $NON_RT_CPUS..."
    for irq in /proc/irq/*; do
         if [ -d "$irq" ]; then
             # Affordable 3 (0011 binary) -> CPU 0,1
             echo 3 > "$irq/smp_affinity" 2>/dev/null
         fi
    done
```

```text
    # 3. Move RCU callbacks (requires kernel support)
    for rcu in /proc/sys/kernel/rcu_nocbs; do
         echo "Checking RCU offload..."
         # Configured via bootargs
    done
```

# 4. Verification

## Page 4

```text
    echo "=== Verification ==="
    echo "Interrupts on isolated CPUs:"
    cat /proc/interrupts | grep -E "CPU2|CPU3" | head -n 5
```

echo "Done. Run your RT app with: taskset -c 2-3 "

4. Cgroups v2 Resource Control

### 3.1 Tại sao Taskset là chưa đủ?

`taskset` chỉ set affinity cho process tại thời điểm gọi. Child process có thể bị thay đổi. Cgroups (Control Groups) cung cấp cơ chế mạnh mẽ và bền vững hơn.

### 3.2 Tạo RT Cgroup Partition

```text
    # Mount cgroup v2
    mount -t cgroup2 none /sys/fs/cgroup
```

```text
    # Tạo group cho RT
    mkdir /sys/fs/cgroup/rt_domain
    # Gán Core 2,3 cho group này
    echo "2-3" > /sys/fs/cgroup/rt_domain/cpuset.cpus
    # Các task trong group này có quyền sử dụng RT Policy
    echo "infinity" > /sys/fs/cgroup/rt_domain/cpu.rt_runtime_us
```

```text
    # Move shell hiện tại vào group để test
    echo $$ > /sys/fs/cgroup/rt_domain/cgroup.procs
```

4. Tổng kết

Cô lập CPU là bước quan trọng nhất để đạt được "Deterministic Latency". Bằng cách dọn dẹp sạch sẽ môi trường (Housekeeping), ta đảm bảo RT Task có thể chạy ngay lập tức khi cần mà không bị cản trở bởi bất kỳ tác vụ nền nào của hệ điều hành.

HALA Academy | Biên soạn: Phạm Văn Vũ
