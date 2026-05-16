# System Architect ARM Cortex-A

# Bai 7.1 - 7.3: Advanced Real-Time Linux, CPU Isolation, and RT Workload Design

**Bien soan:** Pham Van Vu  
**Nguon:** HALA Academy - System Architect ARM Cortex A

> Ghi chu chuyen doi: Tat ca hinh anh trong PDF/anh upload da duoc chuyen thanh **text diagram** trong Markdown de tranh loi Mermaid parser. Khong dung Mermaid.

---

## Muc luc

- [Bai 7.1: Advanced Real-Time Linux Analysis](#bai-71-advanced-real-time-linux-analysis)
  - [Muc tieu bai hoc](#muc-tieu-bai-hoc-71)
  - [1. PREEMPT_RT Architecture Deep Dive](#1-preempt_rt-architecture-deep-dive)
  - [2. Ftrace Latency Analysis](#2-ftrace-latency-analysis)
  - [3. Hardware Latency Detection](#3-hardware-latency-detection)
  - [4. Lab: Detecting Hardware Latency](#4-lab-detecting-hardware-latency)
  - [5. Tong ket](#5-tong-ket-71)
- [Bai 7.2: Advanced CPU Isolation and Housekeeping](#bai-72-advanced-cpu-isolation-and-housekeeping)
  - [Muc tieu bai hoc](#muc-tieu-bai-hoc-72)
  - [1. CPU Housekeeping Strategy](#1-cpu-housekeeping-strategy)
  - [2. Cau hinh Isolation chi tiet](#2-cau-hinh-isolation-chi-tiet)
  - [3. Lab: Scripting the Isolation](#3-lab-scripting-the-isolation)
  - [4. Cgroups v2 Resource Control](#4-cgroups-v2-resource-control)
  - [5. Tong ket](#5-tong-ket-72)
- [Bai 7.3: Real-Time Software Design Patterns](#bai-73-real-time-software-design-patterns)
  - [Muc tieu bai hoc](#muc-tieu-bai-hoc-73)
  - [1. The Priority Inversion Problem](#1-the-priority-inversion-problem)
  - [2. Lock-free Communication Pattern](#2-lock-free-communication-pattern)
  - [3. Memory Management](#3-memory-management)
  - [4. Lab: Stress Testing with Cyclictest](#4-lab-stress-testing-with-cyclictest)
  - [5. Tong ket](#5-tong-ket-73)

---

# Bai 7.1: Advanced Real-Time Linux Analysis

## Muc tieu bai hoc {#muc-tieu-bai-hoc-71}

Sau bai hoc nay, hoc vien can nam duoc:

- Phan tich chi tiet cach **PREEMPT_RT patch** bien Linux thanh he dieu hanh co tinh chat Hard Real-Time hon.
- Su dung **Ftrace** voi cac tracer nhu `irqsoff`, `wakeup_rt` de debug latency o cap do microsecond.
- Phat hien cac nguon tre phan cung nhu **SMI**, cache miss bang `hwlatdetect`.

---

## 1. PREEMPT_RT Architecture Deep Dive

### 1.1 Tu Soft RT den Hard RT

Mot kernel Linux tieu chuan, hay con goi la Vanilla Kernel, chu yeu cung cap **Soft Real-Time**.
Ngay ca khi dung scheduler uu tien nhu `SCHED_FIFO`, nhieu vung code trong kernel, driver, filesystem van co the disable preemption trong thoi gian dai, vi du khi giu `spinlock`.

**PREEMPT_RT** thay doi hanh vi nay bang cach:

- Chuyen doi hau het **spinlock** thanh **mutex co the sleep** hay **sleeping spinlock**.
- Cho phep task uu tien cao hon co the preempt kernel code dang giu lock trong nhieu truong hop.
- Chuyen **IRQ handlers** thanh **kernel threads**.
- Cho phep IRQ threads cung bi lap lich theo priority-based scheduling.

### Hinh 1: PREEMPT_RT vs Standard Kernel

```text
                          PREEMPT_RT vs Standard Kernel

+--------------------------------------------------------------------+
|                          Standard Linux                            |
|--------------------------------------------------------------------|
|  +----------------+    +-------------+    +-------------+          |
|  | IRQ (hardirq)  |    |  Softirq    |    |  Spinlock   |          |
|  +----------------+    +-------------+    +-------------+          |
|  Notes:                                                            |
|  - IRQ runs in interrupt context                                   |
|  - Cannot preempt while holding spinlock                           |
|  - Latency is commonly at ms-level in bad cases                    |
+--------------------------------------------------------------------+

+--------------------------------------------------------------------+
|                             PREEMPT_RT                             |
|--------------------------------------------------------------------|
|  +----------------+    +----------------+    +-------------------+ |
|  |  IRQ Thread    |    | Softirq Thread |    | Sleeping Spinlock | |
|  |                |    |                |    |    (rt_mutex)     | |
|  +----------------+    +----------------+    +-------------------+ |
|  Notes:                                                            |
|  - IRQ runs as kernel thread                                       |
|  - Kernel can be preempted in more places                          |
|  - Latency can reach us-level when system is tuned correctly       |
+--------------------------------------------------------------------+
```

### Standard Linux vs PREEMPT_RT - conceptual comparison

| Area | Standard Linux | PREEMPT_RT |
|---|---|---|
| IRQ handling | Hard IRQ context | IRQ thread context |
| Softirq | Often deferred but still can affect latency | Threaded and schedulable |
| Spinlock | Non-sleeping, disables preemption | Converted to `rt_mutex` in most cases |
| Priority handling | Less deterministic inside kernel critical sections | Priority-aware scheduling inside more kernel paths |
| Typical worst-case latency | Higher, often ms-level in stressed cases | Lower, can be us-level when tuned |

---

## 2. Ftrace Latency Analysis

### 2.1 Anatomy of Latency

Tong do tre tu khi su kien phan cung xay ra den khi RT task duoc chay co the chia thanh:

```text
Latency = T_hardware + T_interrupt + T_sched + T_context_switch
```

Trong do:

- `T_hardware`: Thoi gian CPU mat de nhan IRQ. Bi anh huong boi SMI, bus contention, cache, firmware.
- `T_interrupt`: Thoi gian kernel entry va masked interrupt time.
- `T_sched`: Thoi gian scheduler chon task se chay tiep theo.
- `T_context_switch`: Thoi gian save/restore context va chuyen sang task moi.

### Hinh 2: Wakeup Latency Components

```text
                       Wakeup Latency Trace Analysis

 Hardware Event
      |
      v
+-------------------+      +--------------------+      +-------------------+
| T_hardware        | ---> | T_interrupt        | ---> | T_sched           |
| - IRQ arrives     |      | - IRQ entry        |      | - pick next task  |
| - bus/SMI/cache   |      | - masked IRQ time  |      | - priority check  |
+-------------------+      +--------------------+      +-------------------+
                                                              |
                                                              v
                                                   +------------------------+
                                                   | T_context_switch       |
                                                   | - switch to RT thread  |
                                                   +------------------------+
                                                              |
                                                              v
                                                        RT task runs
```

### 2.2 Su dung `wakeup_rt` tracer

De bat khoanh khac worst-case latency, ta dung Ftrace built-in trong kernel.

```bash
# Bat tracer theo doi do tre wakeup cua RT tasks
echo wakeup_rt > /sys/kernel/debug/tracing/current_tracer
echo 1 > /sys/kernel/debug/tracing/tracing_on

# Chay test load
cyclictest -p 90 -m -t1 -n -i 1000 -l 10000

# Xem ket qua
cat /sys/kernel/debug/tracing/trace | head -n 20
```

Tracer log co the chi ra ham nao trong kernel gay block scheduling, vi du mot driver giu lock qua lau.

### 2.3 Cac tracer thuong dung trong RT debugging

| Tracer | Muc dich |
|---|---|
| `wakeup_rt` | Tim do tre wakeup lon nhat cua RT task |
| `irqsoff` | Tim vung code tat interrupt qua lau |
| `preemptoff` | Tim vung code disable preemption qua lau |
| `preemptirqsoff` | Ket hop preempt-off va irq-off latency |
| `function_graph` | Ve call graph va thoi gian tung function |

---

## 3. Hardware Latency Detection

### 3.1 System Management Interrupts (SMI)

**SMI** la mot trong cac nguon latency nguy hiem nhat cho real-time. Firmware, BIOS, UEFI, hoac logic quan ly nguon co the chiem CPU de xu ly nhiet do, nguon, fan, security, hoac cac tac vu firmware khac.

Trong thoi gian SMI:

- OS khong biet CPU dang bi chiem.
- Moi IRQ co the bi chan.
- RT task khong the duoc schedule dung han.
- Ftrace trong OS co the khong thay ro nguyen nhan vi latency nam ngoai tam nhin kernel.

### 3.2 Cong cu `hwlatdetect`

`hwlatdetect` chay mot kernel thread vong lap kin, so sanh timestamp de phat hien cac khoang thoi gian CPU bi mat tich.

```bash
# Cai dat tu goi rt-tests
sudo apt install rt-tests

# Chay detect trong 60 giay, nguong canh bao 10us
sudo hwlatdetect --duration=60 --threshold=10
```

Neu phat hien SMI latency cao, giai phap co the la:

- Update BIOS/UEFI/firmware.
- Tat cac tinh nang power management gay SMI neu co option.
- Patch hoac cau hinh lai driver lien quan.
- Doi hardware/board neu firmware khong the sua.

---

## 4. Lab: Detecting Hardware Latency

Muc tieu lab: quet toan bo he thong de tim cac nguon latency an, vi du SMI, trong 2 phut.

### Script: `hwlatdetect.sh`

```bash
#!/bin/bash
# hwlatdetect.sh

DURATION=120     # 2 minutes
WINDOW=1000000   # 1s window

echo "Starting Hardware Latency Detector..."
echo "Please do not touch the system..."

sudo hwlatdetect \
  --duration=$DURATION \
  --window=$WINDOW \
  --width=500000 \
  --threshold=10 \
  --hardlimit=20 \
  --report=hwlat_report.txt

if [ -f hwlat_report.txt ]; then
  cat hwlat_report.txt
  echo "Check hwlat_report.txt for details."
else
  echo "No latency spikes detected above threshold."
fi
```

### Cach doc ket qua

| Ket qua | Y nghia | Huong xu ly |
|---|---|---|
| Khong co spike tren threshold | Hardware tuong doi on | Tiep tuc test voi workload that |
| Spike thap, it xay ra | Co the chap nhan tuy deadline | Tang thoi gian test, test nhieu dieu kien |
| Spike lon lap lai | Co nguon latency phan cung/firmware | Kiem tra BIOS, power, thermal, driver |
| Spike lon khi I/O hoac network | Driver/I/O gay tre | Doi IRQ affinity, isolate CPU, tune driver |

---

## 5. Tong ket {#5-tong-ket-71}

De dat duoc Hard Real-Time, chi bat `CONFIG_PREEMPT_RT` la chua du. Ky su he thong can biet cach:

- Phan tich latency bang Ftrace.
- Dung `cyclictest` de do worst-case latency.
- Dung `hwlatdetect` de phat hien latency phan cung/firmware.
- Giam thoi gian disable IRQ/preemption trong driver.
- Dieu chinh IRQ affinity va CPU isolation.

---

# Bai 7.2: Advanced CPU Isolation and Housekeeping

## Muc tieu bai hoc {#muc-tieu-bai-hoc-72}

Sau bai hoc nay, hoc vien can nam duoc:

- Thiet lap mo hinh **Housekeeping CPUs** cho he thong multi-core.
- Su dung **Cgroups v2 cpuset controller** de quan ly tai nguyen CPU.
- Toi uu hoa **Timer Tick** voi `nohz_full`.

---

## 1. CPU Housekeeping Strategy

### 1.1 Khai niem Housekeeping

Trong he thong 4 cores nhu Allwinner H618, khong phai core nao cung nen chay real-time. Ta chia CPU thanh hai nhom:

- **Housekeeping CPUs (Core 0-1)**:
  - Chay OS services.
  - Chay SSH, logging, disk I/O, GUI.
  - Nhan IRQ phan cung thong thuong.
  - Xu ly RCU callbacks va background kernel work.

- **Isolated CPUs (Core 2-3)**:
  - Duoc lam sach de chay RT loop.
  - Khong nhan IRQ thong thuong.
  - Khong chay background tasks.
  - Giam hoac tat timer tick voi `nohz_full`.
  - Danh rieng cho workload can deterministic latency.

### Hinh 1: CPU Housekeeping Topology

```text
                   CPU Housekeeping Topology - Quad-core H618

+---------------------------------------------------------------------------------+
|                                 Linux System                                    |
|                                                                                 |
|  +---------------------------+       +--------------------------------------+   |
|  | Housekeeping Domain       |       | Isolated Real-Time Domain            |   |
|  | CPUs: 0-1                 |       | CPUs: 2-3                            |   |
|  |---------------------------|       |--------------------------------------|   |
|  | - SSH                     |       | - RT control loop                    |   |
|  | - Logging                 |       | - Deterministic workload             |   |
|  | - Disk I/O                |       | - No normal IRQs                     |   |
|  | - Network                 |       | - No scheduler load balancing        |   |
|  | - GUI / non-RT services   |       | - nohz_full tick reduction           |   |
|  | - IRQ affinity target     |       | - RCU callbacks offloaded            |   |
|  +---------------------------+       +--------------------------------------+   |
|              ^                                      ^                           |
|              |                                      |                           |
|      irqaffinity=0,1                         isolcpus=2,3                       |
|      rcu_nocbs handled                       nohz_full=2,3                      |
|      by CPU0-CPU1                            RT app via taskset/cgroup          |
|                                                                                 |
+---------------------------------------------------------------------------------+
```

### Practical goal

Muc tieu khong phai lam cho tat ca CPU real-time, ma la **don sach mot tap CPU rieng** de RT workload co the chay ngay khi can, khong bi OS background workload lam nhieu.

---

## 2. Cau hinh Isolation chi tiet

### 2.1 Kernel Boot Parameters

Them cac tham so sau vao `bootargs`:

```text
isolcpus=2,3
nohz_full=2,3
rcu_nocbs=2,3
irqaffinity=0,1
```

Giai thich:

| Boot parameter | Y nghia |
|---|---|
| `isolcpus=2,3` | Scheduler khong tu y day task thuong vao CPU 2 va 3 |
| `nohz_full=2,3` | Tat periodic timer tick tren CPU 2 va 3 neu dieu kien phu hop |
| `rcu_nocbs=2,3` | Day RCU callbacks khoi CPU 2 va 3 ve CPU housekeeping |
| `irqaffinity=0,1` | Mac dinh IRQ phan cung chi route vao CPU 0 va 1 |

### Vi du bootargs

```text
console=ttyS0,115200 root=/dev/mmcblk0p2 rootfstype=ext4 rw rootwait \
isolcpus=2,3 nohz_full=2,3 rcu_nocbs=2,3 irqaffinity=0,1
```

### 2.2 Dich chuyen IRQ (IRQ Migration)

Du da set `irqaffinity`, mot so driver van co the request IRQ tren CPU isolated. Ta can ep IRQ ve CPU 0-1.

#### Script: `force_irq_migration.sh`

```bash
#!/bin/bash
# force_irq_migration.sh

for irq in /proc/irq/*; do
  if [ -d "$irq" ]; then
    # Mask 0x3 = binary 0011 = CPU0 and CPU1
    echo 3 > "$irq/smp_affinity" 2>/dev/null
  fi
done
```

### 2.3 Verification checklist

```bash
# Xem interrupt co dang tang tren CPU2/CPU3 khong
cat /proc/interrupts

# Xem command line da co isolcpus/nohz_full chua
cat /proc/cmdline

# Xem CPU affinity cua process
ps -eo pid,psr,comm,args

# Chay RT app tren CPU 2-3
taskset -c 2-3 ./rt_app
```

---

## 3. Lab: Scripting the Isolation

Script duoi day tu dong hoa quy trinh co lap CPU va di chuyen IRQ. Luu thanh `setup_isolation.sh` va chay tren board.

### Script: `setup_isolation.sh`

```bash
#!/bin/bash
# setup_isolation.sh: Automate CPU Isolation for H618

RT_CPUS="2-3"
NON_RT_CPUS="0-1"

echo "=== Starting CPU Isolation Setup ==="

# 1. Disable IRQ Balance
if systemctl is-active --quiet irqbalance; then
  echo "[*] Stopping irqbalance..."
  systemctl stop irqbalance
fi

# 2. Migrate IRQs to Housekeeping CPUs (0-1)
echo "[*] Migrating IRQs to CPUs $NON_RT_CPUS..."
for irq in /proc/irq/*; do
  if [ -d "$irq" ]; then
    # 3 = 0011 binary = CPU0 and CPU1
    echo 3 > "$irq/smp_affinity" 2>/dev/null
  fi
done

# 3. Move RCU callbacks (requires kernel support)
for rcu in /proc/sys/kernel/rcu_nocbs; do
  echo "Checking RCU offload..."
  # Configured via bootargs
  true
done

# 4. Verification
echo "=== Verification ==="
echo "Interrupts on isolated CPUs:"
cat /proc/interrupts | grep -E "CPU2|CPU3" | head -n 5

echo "Done. Run your RT app with: taskset -c 2-3 <app>"
```

### Recommended validation after running script

```bash
# CPU list and online status
lscpu
cat /sys/devices/system/cpu/online

# Interrupt distribution
watch -n 1 cat /proc/interrupts

# Check nohz_full boot arg
cat /proc/cmdline | tr ' ' '\n' | grep -E 'isolcpus|nohz_full|rcu_nocbs|irqaffinity'
```

---

## 4. Cgroups v2 Resource Control

### 4.1 Tai sao `taskset` la chua du?

`taskset` chi set CPU affinity cho process tai thoi diem goi. Trong he thong lon:

- Child process co the can policy rieng.
- Service manager co the restart process.
- Can quan ly ca nhom process, khong chi mot PID.
- Can gan tai nguyen CPU, memory, IO mot cach ben vung hon.

**Cgroups v2** cung cap co che manh hon de quan ly theo nhom.

### 4.2 Tao RT Cgroup Partition

```bash
# Mount cgroup v2
mount -t cgroup2 none /sys/fs/cgroup

# Tao group cho RT
mkdir /sys/fs/cgroup/rt_domain

# Gan Core 2,3 cho group nay
echo "2-3" > /sys/fs/cgroup/rt_domain/cpuset.cpus

# Cac task trong group nay co quyen su dung RT Policy
echo "infinity" > /sys/fs/cgroup/rt_domain/cpu.rt_runtime_us

# Move shell hien tai vao group de test
echo $$ > /sys/fs/cgroup/rt_domain/cgroup.procs
```

### 4.3 Example: run RT app inside RT domain

```bash
# Move target app PID into cgroup
echo $PID > /sys/fs/cgroup/rt_domain/cgroup.procs

# Or start shell inside the domain, then run app
echo $$ > /sys/fs/cgroup/rt_domain/cgroup.procs
chrt -f 90 taskset -c 2 ./rt_app
```

---

## 5. Tong ket {#5-tong-ket-72}

CPU isolation la buoc quan trong de dat **Deterministic Latency**. Bang cach don dep housekeeping workload ve CPU 0-1, RT task tren CPU 2-3 co the:

- It bi interrupt chen ngang.
- It bi kernel background work anh huong.
- Giam scheduler noise.
- Giam timer tick noise.
- Dat worst-case latency on dinh hon.

---

# Bai 7.3: Real-Time Software Design Patterns

## Muc tieu bai hoc {#muc-tieu-bai-hoc-73}

Sau bai hoc nay, hoc vien can nam duoc:

- Ap dung cac **design patterns** an toan cho real-time.
- Giai quyet **Priority Inversion** bang **PI Mutex**.
- Thiet ke co che giao tiep **lock-free** giua RT thread va Non-RT thread.

---

## 1. The Priority Inversion Problem

### 1.1 Kich ban tham hoa: Mars Pathfinder

**Priority Inversion** xay ra khi:

1. Task uu tien thap (**Low**) nam giu mot lock.
2. Task uu tien cao (**High**) can lock do nen bi block.
3. Task uu tien trung binh (**Medium**) preempt Low.
4. High bi cho vo thoi han vi Low khong duoc chay de unlock.

Ket qua: Task High co priority cao nhat nhung lai bi Medium chan gian tiep.

### Hinh 1: Priority Inversion and Priority Inheritance Solution

![alt text](<1. PDFs/Diagrams/7.3_priority_inversion_solution.png>)

```text
Participants:
  L  = Low Priority Task
  M  = Medium Priority Task
  H  = High Priority Task
  PI = Priority Inheritance Mutex

Time flows downward.

NORMAL EXECUTION
----------------


INVERSION SCENARIO
------------------


PRIORITY INHERITANCE
--------------------


Effect:
  - Kernel temporarily upgrades L to H's priority.
  - L runs immediately and releases the lock.
  - H gets the lock quickly.
  - M cannot indefinitely delay H through L.
```


### 1.2 Giai phap: Priority Inheritance Mutex

Trong Linux Pthreads, kich hoat Priority Inheritance protocol nhu sau:

```c
pthread_mutexattr_t attr;
pthread_mutexattr_init(&attr);

// Enable Priority Inheritance Protocol
pthread_mutexattr_setprotocol(&attr, PTHREAD_PRIO_INHERIT);

pthread_mutex_t my_mutex;
pthread_mutex_init(&my_mutex, &attr);
```

### When to use PI Mutex

| Case | Use PI Mutex? | Reason |
|---|---:|---|
| RT thread shares lock with lower-priority thread | Yes | Prevent priority inversion |
| Lock is only used by equal-priority RT tasks | Maybe | Depends on design |
| Lock protects logging/file I/O | Avoid | RT loop should not do blocking I/O |
| Lock is in hot path with strict deadline | Prefer lock-free | Mutex can still block |

---

## 2. Lock-free Communication Pattern

### 2.1 Quy tac vang: Khong I/O trong RT Loop

Task Real-Time, vi du priority 99, **tuyet doi khong nen** goi cac ham khong deterministic:

- `printf`
- `write()` to file
- disk I/O
- network I/O blocking
- `malloc()` / `free()` trong hot path
- function co the page fault
- function co the lock global mutex

Ly do: cac ham nay co the sleep, block, trigger page fault, hoac gay latency khong du doan duoc.

### 2.2 Producer-Consumer qua Ring Buffer

Giai phap cho logging/data export trong RT system:

- **RT Thread (Producer)**:
  - Ghi data vao ring buffer.
  - Khong dung mutex trong hot path.
  - Neu buffer day, drop data.
  - Chap nhan mat log con hon tre deadline.

- **Logger Thread (Consumer - Non RT)**:
  - Chay background.
  - Doc ring buffer.
  - Ghi xuong disk/network/UI.

### Text diagram: RT producer and non-RT consumer

```text
+----------------------+       lock-free push        +----------------------+
| RT Thread            | --------------------------> | Ring Buffer          |
| Priority: high       |                             | preallocated memory  |
| Deadline critical    |                             | no malloc in loop    |
+----------------------+                             +----------------------+
                                                              |
                                                              | read/drain
                                                              v
                                                    +----------------------+
                                                    | Logger Thread        |
                                                    | Priority: normal     |
                                                    | printf/write/file IO |
                                                    +----------------------+

Rule:
  RT path must never wait for logger.
  If ring buffer is full, RT path drops data and continues.
```

### Pseudocode: Lock-free Ring Buffer push

```c
bool push(struct ring_buffer *rb, struct data_item item)
{
    unsigned int next_head = (rb->head + 1) % SIZE;

    if (next_head == rb->tail)
        return false; // Full

    rb->buffer[rb->head] = item;

    // Memory barrier: data must be visible before publishing index
    __sync_synchronize();

    rb->head = next_head;
    return true;
}
```

### Design notes

- Ring buffer memory should be allocated during init.
- Avoid dynamic allocation inside RT loop.
- Prefer single-producer/single-consumer design if possible.
- Use memory barriers carefully.
- Decide explicitly whether to drop newest or oldest data when full.

---

## 3. Memory Management

### 3.1 Stack and Heap

#### Stack

- Stack size should be fixed and known.
- Pre-fault stack before entering RT loop.
- Avoid deep recursion.
- Avoid large stack arrays in RT path.

#### Heap

- Avoid `malloc()` / `free()` in RT hot path.
- Use **memory pools**.
- Allocate one large block during init.
- Split into fixed-size objects.
- Return objects to pool without calling general-purpose allocator.

### 3.2 Page Fault Prevention

Dung `mlockall()` de khoa virtual memory vao RAM vat ly, tranh kernel phai load page trong luc RT task dang chay.

```c
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>

if (mlockall(MCL_CURRENT | MCL_FUTURE) == -1) {
    perror("mlockall failed");
    exit(1);
}
```

### RT memory checklist

| Item | Rule |
|---|---|
| Page fault | Avoid after RT loop starts |
| Heap allocation | Preallocate |
| Stack | Pre-fault and keep bounded |
| Logging buffer | Preallocate ring buffer |
| DMA buffer | Allocate before deadline-critical phase |
| File I/O | Move to non-RT thread |

---

## 4. Lab: Stress Testing with Cyclictest

Muc tieu: Danh gia do on dinh cua he thong duoi tai nang bang `stress-ng` va `cyclictest`.

### Script: `run_cyclictest.sh`

```bash
#!/bin/bash
# run_cyclictest.sh

# 1. Generate Load (CPU, IO, VM)
echo "Starting Stress-ng (Background load)..."
stress-ng --cpu 2 --io 2 --vm 1 --vm-bytes 128M --timeout 60s &

# 2. Run Cyclictest on Isolated CPU 3
echo "Starting Cyclictest on CPU 3 (Priority 99)..."
sudo cyclictest \
  --mlockall \
  --smp \
  --priority=99 \
  --interval=200 \
  --distance=0 \
  --affinity=3 \
  --loops=100000 \
  --histogram=100 \
  | tee cyclic_result.txt

# 3. Analysis
MAX_LAT=$(grep "Max Latencies" cyclic_result.txt | awk '{print $NF}')
echo "Max Latency measured: ${MAX_LAT} us"

if [ "$MAX_LAT" -lt 50 ]; then
  echo "PASS: Hard Real-Time Ready!"
else
  echo "FAIL: Optimize more!"
fi
```

### Cach doc ket qua cyclictest

| Metric | Y nghia |
|---|---|
| Min latency | Do tre tot nhat quan sat duoc |
| Avg latency | Do tre trung binh |
| Max latency | Worst-case latency quan trong nhat |
| Histogram | Phan bo latency |
| Spike lap lai | Dau hieu co nguon latency he thong |

### Debug khi Max Latency cao

```text
If Max Latency is high:

1. Check /proc/interrupts
   -> IRQ co dang vao isolated CPU khong?

2. Check bootargs
   -> isolcpus/nohz_full/rcu_nocbs/irqaffinity da dung chua?

3. Check Ftrace
   -> wakeup_rt, irqsoff, preemptoff

4. Check hardware latency
   -> hwlatdetect

5. Check RT app design
   -> co malloc/printf/file I/O/mutex trong RT loop khong?
```

---

## 5. Tong ket {#5-tong-ket-73}

Lap trinh real-time doi hoi ky luat nghiem ngat. Do on dinh cua he thong phu thuoc rat lon vao design pattern:

- Dung **PI Mutex** khi bat buoc share lock giua task khac priority.
- Uu tien **lock-free queues/ring buffers** cho giao tiep RT -> non-RT.
- Dung **memory pools** thay vi malloc trong RT loop.
- Dung `mlockall()` de tranh page fault.
- Khong lam blocking I/O trong RT path.
- Do worst-case latency bang `cyclictest`, Ftrace, va `hwlatdetect`.

---

# Appendix A: One-page Real-Time Checklist

```text
Real-Time Linux Checklist
=========================

Kernel:
  [ ] CONFIG_PREEMPT_RT enabled if hard RT is required
  [ ] Ftrace enabled
  [ ] rt-tests installed

Bootargs:
  [ ] isolcpus=2,3
  [ ] nohz_full=2,3
  [ ] rcu_nocbs=2,3
  [ ] irqaffinity=0,1

CPU/IRQ:
  [ ] irqbalance stopped
  [ ] IRQs moved to housekeeping CPUs
  [ ] RT task pinned to isolated CPU
  [ ] /proc/interrupts verified

Memory:
  [ ] mlockall(MCL_CURRENT | MCL_FUTURE)
  [ ] no malloc/free in RT loop
  [ ] stack pre-faulted
  [ ] memory pools used

Software design:
  [ ] no printf/file I/O/network blocking in RT loop
  [ ] PI mutex used for shared priority locks
  [ ] lock-free ring buffer used for logging/data export

Measurement:
  [ ] cyclictest under stress-ng
  [ ] wakeup_rt trace captured
  [ ] irqsoff/preemptoff checked
  [ ] hwlatdetect run for hidden hardware latency
```

---

# Appendix B: Command Summary

```bash
# Ftrace wakeup_rt
echo wakeup_rt > /sys/kernel/debug/tracing/current_tracer
echo 1 > /sys/kernel/debug/tracing/tracing_on
cyclictest -p 90 -m -t1 -n -i 1000 -l 10000
cat /sys/kernel/debug/tracing/trace | head -n 20

# Hardware latency detection
sudo hwlatdetect --duration=60 --threshold=10

# Force IRQs to CPU0-CPU1
for irq in /proc/irq/*; do
  if [ -d "$irq" ]; then
    echo 3 > "$irq/smp_affinity" 2>/dev/null
  fi
done

# Run RT app on isolated CPUs
taskset -c 2-3 ./rt_app

# Run cyclictest on CPU3
sudo cyclictest --mlockall --smp --priority=99 --interval=200 \
  --distance=0 --affinity=3 --loops=100000 --histogram=100
```

---

# Cau hoi on tap tong hop

1. PREEMPT_RT thay doi cach kernel xu ly IRQ va spinlock nhu the nao?
2. Tai sao `wakeup_rt` tracer huu ich khi debug latency cua RT task?
3. SMI co nguy hiem gi voi he thong real-time?
4. Khac nhau giua Housekeeping CPU va Isolated CPU?
5. Tai sao `taskset` chua du trong he thong phuc tap?
6. Priority Inversion la gi? PI Mutex giai quyet nhu the nao?
7. Tai sao khong duoc goi `printf`, file I/O, hoac `malloc` trong RT loop?
8. Ring buffer lock-free nen xu ly the nao khi buffer day?
9. `mlockall(MCL_CURRENT | MCL_FUTURE)` giup tranh van de gi?
10. Neu `cyclictest` cho Max Latency cao, can debug theo thu tu nao?