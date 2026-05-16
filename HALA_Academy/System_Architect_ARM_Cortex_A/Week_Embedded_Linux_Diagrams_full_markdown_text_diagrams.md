# Embedded Linux / ARM Cortex-A System Diagrams

> Converted from the uploaded diagrams into one Markdown file.
>
> Rule applied: diagrams are written as plain text diagrams, not Mermaid, to avoid parser errors.

---

# Table of Contents

- [Embedded Linux / ARM Cortex-A System Diagrams](#embedded-linux--arm-cortex-a-system-diagrams)
- [Table of Contents](#table-of-contents)
- [1. OTA / Secure Update / CI-CD](#1-ota--secure-update--ci-cd)
  - [1.1 OTA A/B Partitioning Strategy](#11-ota-ab-partitioning-strategy)
    - [Original diagram meaning](#original-diagram-meaning)
    - [Text diagram](#text-diagram)
    - [Key fields](#key-fields)
    - [Practical notes](#practical-notes)
  - [1.2 DM-Verity Hash Tree Structure](#12-dm-verity-hash-tree-structure)
    - [Original diagram meaning](#original-diagram-meaning-1)
    - [Text diagram](#text-diagram-1)
    - [Security model](#security-model)
    - [Practical notes](#practical-notes-1)
  - [1.3 Automated Embedded Testing Farm](#13-automated-embedded-testing-farm)
    - [Original diagram meaning](#original-diagram-meaning-2)
    - [Text diagram](#text-diagram-2)
    - [Test farm control points](#test-farm-control-points)
- [2. AMP / RTOS / Remoteproc](#2-amp--rtos--remoteproc)
  - [2.1 AMP Memory Layout](#21-amp-memory-layout)
    - [Original diagram meaning](#original-diagram-meaning-3)
    - [Text diagram](#text-diagram-3)
    - [Example memory map](#example-memory-map)
  - [2.2 SMP vs AMP Concept](#22-smp-vs-amp-concept)
    - [Original diagram meaning](#original-diagram-meaning-4)
    - [Text diagram](#text-diagram-4)
  - [2.3 Remoteproc Boot Flow](#23-remoteproc-boot-flow)
    - [Original diagram meaning](#original-diagram-meaning-5)
    - [Text diagram](#text-diagram-5)
    - [Typical userspace commands](#typical-userspace-commands)
    - [Boot responsibilities](#boot-responsibilities)
  - [2.4 Linux ↔ RTOS IPC Flow](#24-linux--rtos-ipc-flow)
    - [Original diagram meaning](#original-diagram-meaning-6)
    - [Text diagram](#text-diagram-6)
    - [Key concept](#key-concept)
- [3. Real-Time / CPU Isolation](#3-real-time--cpu-isolation)
  - [3.1 CPU Isolation for Real-Time](#31-cpu-isolation-for-real-time)
    - [Original diagram meaning](#original-diagram-meaning-7)
    - [Text diagram](#text-diagram-7)
    - [CPU role split](#cpu-role-split)
  - [3.2 RT Task Periodic Loop](#32-rt-task-periodic-loop)
    - [Original diagram meaning](#original-diagram-meaning-8)
    - [Text diagram](#text-diagram-8)
    - [Deterministic loop checklist](#deterministic-loop-checklist)
- [4. Kernel / Boot / Storage / Security](#4-kernel--boot--storage--security)
  - [4.1 Kernel Boot Flow: `start_kernel()` to Userspace](#41-kernel-boot-flow-start_kernel-to-userspace)
    - [Original diagram meaning](#original-diagram-meaning-9)
    - [Text diagram](#text-diagram-9)
    - [Main phases](#main-phases)
  - [4.2 SD Card Image Layout](#42-sd-card-image-layout)
    - [Original diagram meaning](#original-diagram-meaning-10)
    - [Text diagram](#text-diagram-10)
    - [Offset summary](#offset-summary)
    - [Typical boot partition contents](#typical-boot-partition-contents)
  - [4.3 Defense in Depth - Security Layers](#43-defense-in-depth---security-layers)
    - [Original diagram meaning](#original-diagram-meaning-11)
    - [Text diagram](#text-diagram-11)
    - [Defense notes](#defense-notes)
- [5. Graphics / DRM / Multimedia](#5-graphics--drm--multimedia)
  - [5.1 DRM/KMS Object Hierarchy](#51-drmkms-object-hierarchy)
    - [Original diagram meaning](#original-diagram-meaning-12)
    - [Text diagram](#text-diagram-12)
    - [Object notes](#object-notes)
    - [Relationship chain](#relationship-chain)
  - [5.2 Graphics Software Stack](#52-graphics-software-stack)
    - [Original diagram meaning](#original-diagram-meaning-13)
    - [Text diagram](#text-diagram-13)
    - [Layer summary](#layer-summary)
  - [5.3 GStreamer Video Pipeline](#53-gstreamer-video-pipeline)
    - [Original diagram meaning](#original-diagram-meaning-14)
    - [Text diagram](#text-diagram-14)
    - [Example pipeline](#example-pipeline)
  - [5.4 V4L2 Stateless Decode Flow](#54-v4l2-stateless-decode-flow)
    - [Original diagram meaning](#original-diagram-meaning-15)
    - [Text diagram](#text-diagram-15)
    - [Decode responsibilities](#decode-responsibilities)
  - [5.5 Wayland Compositor Flow](#55-wayland-compositor-flow)
    - [Original diagram meaning](#original-diagram-meaning-16)
    - [Text diagram](#text-diagram-16)
    - [Frame budget](#frame-budget)
- [6. IVI System Architecture](#6-ivi-system-architecture)
    - [Original diagram meaning](#original-diagram-meaning-17)
    - [Text diagram](#text-diagram-17)
    - [Layer responsibilities](#layer-responsibilities)
- [Summary](#summary)

---

# 1. OTA / Secure Update / CI-CD

## 1.1 OTA A/B Partitioning Strategy

### Original diagram meaning

This diagram shows an OTA update strategy using two system slots:

- **System A** is currently active.
- **System B** is inactive and used as the update target.
- The OTA server provides a `.swu` update artifact.
- The SWUpdate agent downloads the artifact and stream-writes it to the inactive slot.
- After the write is complete, the agent updates the U-Boot environment to boot from the new slot.
- Persistent user data is stored separately and is not overwritten during system updates.

### Text diagram

```text
                         OTA A/B PARTITIONING STRATEGY
                         =============================

                           +-------------------------+
                           |        OTA Server       |
                           |-------------------------|
                           | Update Artifact (.swu)  |
                           +------------+------------+
                                        |
                                        | Download
                                        v
                              +-------------------+
                              |  SWUpdate Agent   |
                              +---------+---------+
                                        |
          +-----------------------------+-----------------------------+
          |                                                           |
          | Stream write                                              | Set boot_part=B
          v                                                           v

+--------------------------------------------------------------------------------------+
|                                Storage (eMMC / SD)                                   |
|--------------------------------------------------------------------------------------|
|                                                                                      |
|  +----------------------+  +------------------------------+  +--------------------+  |
|  | Data                 |  | System B (Inactive/Update)   |  | System A (Active)  |  |
|  |----------------------|  |------------------------------|  |--------------------|  |
|  | User Data (Persist)  |  | Rootfs B      Kernel B       |  | Rootfs A Kernel A  |  |
|  +----------------------+  +------------------------------+  +--------------------+  |
|                                                                                      |
|  +------------------------------------+                                              |
|  | Bootloader                         |                                              |
|  |------------------------------------|                                              |
|  | SPL + U-Boot       U-Boot Env      |<---------------------------------------------+
|  +------------------------------------+
|                      |
|                      | boot_count = 0
|                      | upgrade_available = 1
|                      v
+--------------------------------------------------------------------------------------+
```

### Key fields

```text
boot_part = B
boot_count = 0
upgrade_available = 1
```

### Practical notes

- The inactive slot is written first to avoid damaging the running system.
- Bootloader environment variables decide which slot to boot.
- A robust OTA system should support rollback if the new slot fails to boot.
- Persistent user data should be kept outside A/B rootfs partitions.

---

## 1.2 DM-Verity Hash Tree Structure

### Original diagram meaning

This diagram shows how `dm-verity` verifies a read-only root filesystem using a hash tree.

- The **root hash** is trusted.
- The root hash verifies the hash tree metadata.
- The hash tree verifies lower hash layers.
- The final hash layer verifies rootfs data blocks.
- If any block is modified, the hash mismatch causes an I/O error.

### Text diagram

```text
                         DM-VERITY HASH TREE STRUCTURE
                         =============================

                     +--------------------------+
                     | Root Hash (Trusted)      |
                     +-------------+------------+
                                   |
                                   | Verifies
                                   v
                  +----------------------------------+
                  | Hash Tree (Metadata)             |
                  |----------------------------------|
                  |                                  |
                  |   +--------------------------+   |
                  |   | Layer 1 Hashes           |   |
                  |   +------------+-------------+   |
                  |                |                 |
                  |                | Verifies        |
                  |                v                 |
                  |   +--------------------------+   |
                  |   | Layer 2 Hashes           |   |
                  |   +------------+-------------+   |
                  |                |                 |
                  |                | ...             |
                  |                v                 |
                  |   +--------------------------+   |
                  |   | Layer N Hashes           |   |
                  |   +------+-----+------+------+   |
                  |          |     |      |          |
                  +----------|-----|------|----------+
                             |     |      |
                             | Hashes and verifies
                             v     v      v

+------------------------------------------------------+
| Data Blocks (Rootfs)                                 |
|------------------------------------------------------|
|  +---------+    +---------+    +---------+    +-----+ |
|  | Block 0 |    | Block 1 |    | Block 2 |    | ... | |
|  +---------+    +---------+    +---------+    +-----+ |
|                                             +-------+ |
|                                             |Block N| |
|                                             +-------+ |
+------------------------------------------------------+

Root hash trust source:
  - Signed by private key
  - Stored in verified boot metadata such as vbmeta
  - Or passed through trusted kernel command line

Rootfs:
  - Read-only Ext4 / SquashFS
  - Modification causes hash mismatch
  - Hash mismatch causes I/O error
```

### Security model

```text
Trusted root hash
    -> verifies hash tree metadata
        -> verifies rootfs block hashes
            -> verifies actual rootfs data blocks
```

### Practical notes

- `dm-verity` detects tampering but does not encrypt data.
- It is commonly paired with secure boot or verified boot.
- Best suited for immutable system partitions.
- User-writable data should live in a separate partition.

---

## 1.3 Automated Embedded Testing Farm

### Original diagram meaning

This diagram shows a CI/CD board farm for embedded Linux testing.

- Developer pushes code to Git.
- CI pipeline builds artifacts.
- Lab host receives/deploys artifacts.
- GitLab Runner controls LabGrid/PDDU.
- LabGrid controls power, serial, and network access to the DUT.
- The DUT is an Orange Pi Zero 3 board with bootloader and Linux kernel.

### Text diagram

```text
                 AUTOMATED EMBEDDED TESTING FARM (CI/CD)
                 ========================================

+-------------+          Push code
| Developer   |----------------------+
+-------------+                      |
                                     v
+-----------------------------------------------------------------+
| GitLab / CI Server                                              |
|-----------------------------------------------------------------|
|                                                                 |
|  +-------------------+       Trigger build      +------------+  |
|  | Git Repository    |------------------------->| CI Pipeline|  |
|  |                   |                          | (Docker)   |  |
|  +-------------------+                          +-----+------+  |
|                                                        |        |
+--------------------------------------------------------|--------+
                                                         |
                                                         | Deploy artifacts
                                                         v
+----------------------------------------------------------------+
| Lab Host (Testing Server)                                      |
|----------------------------------------------------------------|
|                                                                |
|  +-------------------+             +------------------------+  |
|  | Serial Consoles   |             | GitLab Runner          |  |
|  +-------------------+             +-----------+------------+  |
|                                                | Control       |
|                                                v               |
|                                      +----------------------+  |
|                                      | LabGrid / PDDU       |  |
|                                      +----+---------+-------+  |
|                                           |         |          |
+-------------------------------------------|---------|----------+
                                            |         |
                                            |         |
        Power cycle ON/OFF -----------------+         |
        Send commands / read logs --------------------+
        Fastboot / TFTP ----------------------------------------------+
                                                                      |
                                                                      v
+--------------------------------------------------------------------------------+
| Device Under Test (DUT)                                                        |
|--------------------------------------------------------------------------------|
|                                                                                |
|  Power (Relay)          UART (FTDI)             Ethernet                       |
|      o                      o                      o                           |
|                                                                                |
|                                +---------------------------------------------+ |
|                                | Orange Pi Zero 3                            | |
|                                |---------------------------------------------| |
|                                | +--------------+       +------------------+ | |
|                                | | Linux Kernel |       | Bootloader       | | |
|                                | +--------------+       +------------------+ | |
|                                +---------------------------------------------+ |
+--------------------------------------------------------------------------------+
```

### Test farm control points

```text
Power control:
  - Turn board on/off
  - Recover hung systems
  - Validate cold boot

Serial console:
  - Capture boot logs
  - Send U-Boot commands
  - Debug kernel panic

Network:
  - TFTP boot
  - Fastboot or artifact transfer
  - SSH-based test execution
```

---

# 2. AMP / RTOS / Remoteproc

## 2.1 AMP Memory Layout

### Original diagram meaning

This diagram shows an AMP memory layout where Linux and RTOS share DRAM but use separated regions.

- Linux owns the low memory region.
- A shared memory region is reserved for IPC such as TX/RX vrings and data buffers.
- RTOS owns a reserved memory region for code and data.
- The end of RAM is marked at `0x80000000`.

### Text diagram

```text
                         AMP MEMORY LAYOUT
                         =================

+--------------------------------------------------------------------------------+
| DRAM (1GB - 4GB)                                                               |
|--------------------------------------------------------------------------------|
|                                                                                |
|  +----------------------+  +----------------------+  +----------------------+  |
|  | 0x40000000           |  | 0x7F000000           |  | 0x7F800000           |  |
|  | Linux Memory         |  | Shared Memory        |  | RTOS Memory          |  |
|  | Kernel + User        |  | IPC, rpmsg           |  | Code + Data          |  |
|  +----------------------+  +----------------------+  +----------------------+  |
|                                                                                |
|  +----------------------+                                                      |
|  | 0x80000000           |                                                      |
|  | End of RAM           |                                                      |
|  +----------------------+                                                      |
|                                                                                |
+--------------------------------------------------------------------------------+

Region notes:
  Linux Memory:
    - Managed by Linux kernel
    - MMU enabled

  Shared Memory:
    - About 1MB for IPC
    - TX/RX vrings
    - Data buffers

  RTOS Memory:
    - About 8MB reserved
    - no-map in Device Tree
    - Direct access by RTOS
```

### Example memory map

```text
0x40000000  Linux memory start
            Linux kernel + userspace memory
            Managed by Linux MMU

0x7F000000  Shared memory start
            IPC rings, rpmsg vrings, data buffers

0x7F800000  RTOS reserved memory start
            RTOS code and data

0x80000000  End of RAM
```

---

## 2.2 SMP vs AMP Concept

### Original diagram meaning

This diagram contrasts:

- **SMP**: one OS controls multiple CPUs under one scheduler.
- **AMP**: Linux and RTOS each control separate CPU domains and communicate through IPC.

### Text diagram

```text
                         SMP VS AMP CONCEPT
                         ==================

SMP: Symmetric Multi-Processing
-------------------------------

+--------------------------------------+
| One OS / One Scheduler               |
|--------------------------------------|
|                                      |
|                +-------+             |
|                | CPU 0 |             |
|                +---+---+             |
|                    |                 |
|       +------------+------------+    |
|       |            |            |    |
|       v            v            v    |
|    +-----+      +-----+      +-----+ |
|    |CPU 1|      |CPU 2|      |CPU 3| |
|    +-----+      +-----+      +-----+ |
|                                      |
+--------------------------------------+

Characteristics:
  - One Linux kernel manages all CPUs
  - Shared scheduler
  - Shared memory model
  - Good for general-purpose workloads


AMP: Asymmetric Multi-Processing
--------------------------------

+------------------------------------------------+
| Split OS Domains                               |
|------------------------------------------------|
|                                                |
|  Linux domain                  RTOS domain     |
|  +--------+                    +--------+      |
|  | CPU 0  |<------ IPC ------->| CPU 3  |      |
|  +---+----+                    +---+----+      |
|      |                             |           |
|      v                             v           |
|  +--------+                    +--------+      |
|  | CPU 1  |                    | RT Task|      |
|  +--------+                    +--------+      |
|                                                |
|  +--------+                                    |
|  | CPU 2  |                                    |
|  +--------+                                    |
+------------------------------------------------+

Characteristics:
  - Linux and RTOS run side by side
  - RTOS owns dedicated CPU or CPU cluster
  - Communication through shared memory / rpmsg
  - Good for deterministic real-time control
```

---

## 2.3 Remoteproc Boot Flow

### Original diagram meaning

This diagram shows how Linux boots an RTOS firmware on another CPU using the `remoteproc` framework.

### Text diagram

```text
                         REMOTEPROC BOOT FLOW
                         ====================

                              ●
                              |
                              v
+------------------------------------------------+
| Linux Kernel                                   |
|------------------------------------------------|
|  Boot Linux on CPU 0-2                         |
|        |                                       |
|        v                                       |
|  Load remoteproc driver                        |
|        |                                       |
|        v                                       |
|  Parse Device Tree reserved-memory             |
+--------+---------------------------------------+
         |
         v
+------------------------------------------------+
| Userspace                                      |
|------------------------------------------------|
|  echo "rtos.elf" > /sys/class/remoteproc/.../firmware |
|        |                                       |
|        v                                       |
|  echo "start" > /sys/class/remoteproc/.../state|
+--------+---------------------------------------+
         |
         v
+------------------------------------------------+
| Remoteproc Driver                              |
|------------------------------------------------|
|  Request firmware (rtos.elf)                   |
|        |                                       |
|        v                                       |
|  Parse resource table                          |
|        |                                       |
|        v                                       |
|  Load ELF segments to 0x7F800000               |
|        |                                       |
|        v                                       |
|  Setup virtio devices (vdev)                   |
|        |                                       |
|        v                                       |
|  Reset CPU 3                                   |
+--------+---------------------------------------+
         |
         v
+------------------------------------------------+
| RTOS (CPU 3)                                   |
|------------------------------------------------|
|  Reset handler                                 |
|        |                                       |
|        v                                       |
|  Initialize hardware                           |
|        |                                       |
|        v                                       |
|  Create IPC task                               |
|        |                                       |
|        v                                       |
|  Wait for link up                              |
+--------+---------------------------------------+
         |
         v
         ◎
```

### Typical userspace commands

```bash
echo "rtos.elf" > /sys/class/remoteproc/remoteproc0/firmware
echo "start" > /sys/class/remoteproc/remoteproc0/state
```

### Boot responsibilities

```text
Linux:
  - Owns boot process
  - Loads remoteproc driver
  - Parses reserved memory
  - Loads firmware into RTOS memory

Remoteproc:
  - Parses ELF and resource table
  - Creates virtio/rpmsg channels
  - Releases/reset target CPU

RTOS:
  - Starts from reset handler
  - Initializes local hardware
  - Starts IPC task
  - Waits for Linux link-up
```

---

## 2.4 Linux ↔ RTOS IPC Flow

### Original diagram meaning

This diagram shows a request-response IPC path between a Linux application and an RTOS IPC task through `rpmsg` and shared memory vrings.

### Text diagram

```text
                         LINUX <-> RTOS IPC FLOW
                         =======================

Participants:
  Linux App | rpmsg | Shared Memory | RTOS IPC Task


Linux App           rpmsg              Shared Memory             RTOS IPC Task
    |                 |                       |                        |
    | write(fd, msg)  |                       |                        |
    |---------------->|                       |                        |
    |                 | Copy to TX vring      |                        |
    |                 |---------------------->|                        |
    |                 | Doorbell IRQ          |                        |
    |                 |----------------------------------------------->|
    |                 |                       | Read from vring        |
    |                 |                       |<-----------------------|
    |                 |                       |                        |
    |                 |                       |       Process message  |
    |                 |                       |                        |
    |                 |                       | Write response         |
    |                 |                       |<-----------------------|
    |                 | Doorbell IRQ          |                        |
    |                 |<-----------------------------------------------|
    |                 | Read from RX vring    |                        |
    |                 |---------------------->|                        |
    | return to read()|                       |                        |
    |<----------------|                       |                        |
    |                 |                       |                        |

Shared memory contains:
  - Virtio-based ring buffers
  - TX vring
  - RX vring
  - Optional data buffers
```

### Key concept

```text
Linux App
  -> write() to rpmsg endpoint
    -> rpmsg copies message into TX vring
      -> doorbell IRQ wakes RTOS
        -> RTOS reads and processes message
          -> RTOS writes response into RX vring
            -> doorbell IRQ wakes Linux
              -> Linux read() returns response
```

---

# 3. Real-Time / CPU Isolation

## 3.1 CPU Isolation for Real-Time

### Original diagram meaning

This diagram shows CPU 3 isolated for an RT task, while CPU 0-2 handle general OS work.

### Text diagram

```text
                         CPU ISOLATION FOR REAL-TIME
                         ===========================

+-------------------------------+     +-------------------------------------------------------+
| Isolated CPU                  |     | Non-Isolated CPUs                                     |
|-------------------------------|     |-------------------------------------------------------|
|                               |     |                                                       |
|  +-------------------------+  |     |  +---------------+  +---------------+  +------------+ |
|  | CPU 3                   |  |     |  | CPU 0         |  | CPU 1         |  | CPU 2      | |
|  |-------------------------|  |     |  |---------------|  |---------------|  |------------| |
|  | RT Task Only            |  |     |  | IRQs          |  | IRQs          |  | RCU        | |
|  | No IRQs                 |  |     |  | Kernel threads|  | User apps     |  | callbacks  | |
|  | No timer ticks          |  |     |  | System service|  |               |  | Background | |
|  | No kernel work          |  |     |  |               |  |               |  | work       | |
|  +-------------------------+  |     |  +---------------+  +---------------+  +------------+ |
|                               |     |                                                       |
+-------------------------------+     +-------------------------------------------------------+

Boot arguments:
  isolcpus=3 nohz_full=3 rcu_nocbs=3
```

### CPU role split

| CPU | Role | Workload |
|---:|---|---|
| CPU0 | Housekeeping | IRQs, kernel threads, system services |
| CPU1 | General workload | IRQs, user applications |
| CPU2 | Background workload | RCU callbacks, background work |
| CPU3 | Isolated RT CPU | Real-time task only |

---

## 3.2 RT Task Periodic Loop

### Original diagram meaning

This diagram shows a deterministic real-time task loop using:

- `mlockall()` to lock memory.
- `sched_setscheduler()` to set RT priority.
- `clock_gettime()` to initialize timing.
- `clock_nanosleep(TIMER_ABSTIME)` to sleep until the next absolute period.

### Text diagram

```text
                         RT TASK PERIODIC LOOP
                         =====================

                              ●
                              |
                              v
+----------------------------------------------------------------+
| mlockall(MCL_CURRENT | MCL_FUTURE)                             |
+------------------------------+---------------------------------+
                               |
                               | Lock all memory
                               v
+----------------------------------------------------------------+
| sched_setscheduler(SCHED_FIFO, 80)                             |
+------------------------------+---------------------------------+
                               |
                               | Set RT priority
                               v
+----------------------------------------------------------------+
| clock_gettime(CLOCK_MONOTONIC, &next)                          |
+------------------------------+---------------------------------+
                               |
                               v
                         +-----------+
                         | running ? |
                         +-----+-----+
                               |
                               v
+----------------------------------------------------------------+
| Do RT Work                                                     |
|----------------------------------------------------------------|
| - Control loop                                                 |
| - Sensor read                                                  |
| - Actuator write                                               |
+------------------------------+---------------------------------+
                               |
                               v
+----------------------------------------------------------------+
| next.tv_nsec += PERIOD_NS                                      |
+------------------------------+---------------------------------+
                               |
                               v
+----------------------------------------------------------------+
| clock_nanosleep(TIMER_ABSTIME, &next)                          |
+------------------------------+---------------------------------+
                               |
                               | Sleep until next period
                               v
                         +-----------+
                         | running ? |
                         +-----+-----+
                               |
                               +-------------------- back to Do RT Work
                               |
                               v
                               ◎
```

### Deterministic loop checklist

```text
Before loop:
  - Lock memory with mlockall()
  - Pre-allocate all buffers
  - Avoid malloc/free
  - Avoid printf/logging in RT path
  - Set SCHED_FIFO or SCHED_RR
  - Pin task to isolated CPU

Inside loop:
  - Use absolute time sleep
  - Do bounded work only
  - Avoid blocking I/O
  - Avoid non-PI mutex
  - Drop non-critical data if deadline pressure occurs
```

---

# 4. Kernel / Boot / Storage / Security

## 4.1 Kernel Boot Flow: `start_kernel()` to Userspace

### Original diagram meaning

This sequence diagram shows how U-Boot jumps to the Linux kernel, then Linux executes early assembly, `start_kernel()`, `rest_init()`, `kernel_init`, and finally `/sbin/init`.

### Text diagram

```text
                    KERNEL BOOT FLOW: start_kernel() TO USERSPACE
                    =============================================

Participants:
  U-Boot | head.S | start_kernel() | rest_init() | kernel_init | /sbin/init


U-Boot              head.S          start_kernel()       rest_init()       kernel_init       /sbin/init
  |                   |                  |                  |                 |                |
  | booti(jump_to_text)                  |                  |                 |                |
  |------------------>|                  |                  |                 |                |
  |                   | __primary_switch()                  |                 |                |
  |                   | Enable MMU       |                  |                 |                |
  |                   |----------------->|                  |                 |                |
  |                   | Jump to start_kernel()              |                 |                |
  |                   |----------------->|                  |                 |                |
  |                   |                  | setup_arch()     |                 |                |
  |                   |                  | Parse DTB        |                 |                |
  |                   |                  | mm_init()        |                 |                |
  |                   |                  | sched_init()     |                 |                |
  |                   |                  | early_irq_init() |                 |                |
  |                   |                  | time_init()      |                 |                |
  |                   |                  | console_init()   |                 |                |
  |                   |                  | rest_init()      |                 |                |
  |                   |                  |----------------->|                 |                |
  |                   |                  |                  | kernel_thread(kernel_init)      |
  |                   |                  |                  |---------------->|                |
  |                   |                  |                  |                 | Create PID 1    |
  |                   |                  |                  |                 | kernel_init_freeable()
  |                   |                  |                  |                 | do_basic_setup()
  |                   |                  |                  |                 | Driver probing
  |                   |                  |                  |                 | prepare_namespace()
  |                   |                  |                  |                 | Mount rootfs
  |                   |                  |                  |                 | run_init_process("/sbin/init")
  |                   |                  |                  |                 |--------------->|
  |                   |                  |                  |                 |                | exec() replaces kernel_init
  |                   |                  |                  |                 |                | systemd / busybox init
  |                   |                  |                  |                 |                | PID 1 in userspace
```

### Main phases

```text
U-Boot:
  - Loads Image and DTB
  - Executes booti
  - Jumps to kernel text

head.S:
  - Low-level ARM64 entry
  - Enables MMU
  - Branches to C code

start_kernel():
  - Parses architecture data and DTB
  - Initializes memory, scheduler, IRQ, time, console
  - Calls rest_init()

rest_init():
  - Creates kernel_init thread
  - Creates kthreadd

kernel_init:
  - Runs basic initcalls
  - Probes drivers
  - Mounts root filesystem
  - Executes /sbin/init

/sbin/init:
  - Becomes PID 1 in userspace
  - Usually systemd or BusyBox init
```

---

## 4.2 SD Card Image Layout

### Original diagram meaning

This diagram shows a typical SD card layout for U-Boot SPL, U-Boot proper, boot partition, and root partition.

### Text diagram

```text
                         SD CARD IMAGE LAYOUT
                         ====================

+--------------------------------+
| SD Card (4GB+)                 |
|--------------------------------|
|                                |
|  +--------------------------+  |
|  | MBR                      |  |
|  | 0 - 8KB                  |  |
|  +--------------------------+  |
|                                |
|  +--------------------------+  |
|  | SPL (U-Boot SPL)         |  |
|  | 8KB - 40KB               |  |
|  +--------------------------+  |
|                                |
|  +--------------------------+  |
|  | U-Boot Proper            |  |
|  | 40KB - 1MB               |  |
|  +--------------------------+  |
|                                |
|  +--------------------------+  |
|  | Boot Partition (FAT32)   |  |
|  | 1MB - 128MB              |  |
|  | [Image, *.dtb, boot.scr] |  |
|  +--------------------------+  |
|                                |
|  +--------------------------+  |
|  | Root Partition (ext4)    |  |
|  | 128MB - End              |  |
|  | [/bin, /lib, /etc, /usr] |  |
|  +--------------------------+  |
|                                |
+--------------------------------+
```

### Offset summary

| Offset | Content |
|---|---|
| `0-8KB` | MBR + partition table |
| `8KB-40KB` | U-Boot SPL |
| `40KB-1MB` | U-Boot proper |
| `1MB-128MB` | Boot partition, usually FAT32 |
| `128MB+` | Rootfs, usually ext4 |

### Typical boot partition contents

```text
/Image
/*.dtb
/boot.scr
/extlinux/extlinux.conf    # optional, depending on boot flow
/initramfs.cpio.gz         # optional
```

---

## 4.3 Defense in Depth - Security Layers

### Original diagram meaning

This diagram shows layered security. If an attacker bypasses one layer, the next layer still provides defense.

### Text diagram

```text
                         DEFENSE IN DEPTH - SECURITY LAYERS
                         ===================================

                             Attacker
                                |
                                | Attack Entry
                                v
+----------------------------------------------------------------+
| Application Security                                           |
|----------------------------------------------------------------|
| Input validation, secure coding, authentication                |
+-------------------------------+--------------------------------+
                                |
                                | If breached
                                v
+----------------------------------------------------------------+
| OS Hardening                                                   |
|----------------------------------------------------------------|
| SELinux / AppArmor, minimal services, sysctl                   |
+-------------------------------+--------------------------------+
                                |
                                | If breached
                                v
+----------------------------------------------------------------+
| Kernel Hardening                                               |
|----------------------------------------------------------------|
| KASLR, stack protector, seccomp                                |
+-------------------------------+--------------------------------+
                                |
                                | If breached
                                v
+----------------------------------------------------------------+
| Secure Boot                                                    |
|----------------------------------------------------------------|
| Signed bootloader, verified kernel, dm-verity                  |
+-------------------------------+--------------------------------+
                                |
                                | Final defense
                                v
+----------------------------------------------------------------+
| Hardware Security                                              |
|----------------------------------------------------------------|
| TrustZone, Secure Boot ROM, hardware RNG                       |
+----------------------------------------------------------------+
```

### Defense notes

```text
Application Security:
  - First line of defense
  - Most exposed to attacks
  - Must validate all input

Hardware Security:
  - Trust anchor
  - Hardest to compromise
  - Hardware-based root of trust
```

---

# 5. Graphics / DRM / Multimedia

## 5.1 DRM/KMS Object Hierarchy

### Original diagram meaning

This diagram shows the core DRM/KMS display objects and their relationship.

### Text diagram

```text
                         DRM/KMS OBJECT HIERARCHY
                         ========================

+--------------------------------------+
| KMS Objects                          |
|--------------------------------------|
|                                      |
|  +-------------------------------+   |
|  | Framebuffer (FB)              |   |
|  | Pixel buffer in memory        |   |
|  +---------------+---------------+   |
|                  | attached to       |
|                  v                   |
|  +-------------------------------+   |
|  | Plane                         |   |
|  | Primary, Overlay, Cursor      |   |
|  +---------------+---------------+   |
|                  | composited by     |
|                  v                   |
|  +-------------------------------+   |
|  | CRTC                          |   |
|  | Timing generator              |   |
|  +---------------+---------------+   |
|                  | drives            |
|                  v                   |
|  +-------------------------------+   |
|  | Encoder                       |   |
|  | Signal converter              |   |
|  +---------------+---------------+   |
|                  | outputs to        |
|                  v                   |
|  +-------------------------------+   |
|  | Connector                     |   |
|  | Physical output, e.g. HDMI    |   |
|  +-------------------------------+   |
|                                      |
+--------------------------------------+
```

### Object notes

```text
Plane:
  - Primary: main display
  - Overlay: video, hardware cursor, popup
  - Cursor: mouse pointer

CRTC:
  - Generates timing
  - Handles scanout
  - Multiple planes can feed one CRTC
```

### Relationship chain

```text
Framebuffer
  -> attached to Plane
    -> composited by CRTC
      -> drives Encoder
        -> outputs to Connector
          -> physical display
```

---

## 5.2 Graphics Software Stack

### Original diagram meaning

This diagram shows how graphics applications use OpenGL ES, Vulkan, and EGL through Mesa, DRM, and the Panfrost/sun4i drivers.

### Text diagram

```text
                         GRAPHICS SOFTWARE STACK
                         =======================

+-------------------------------+
| Application                   |
|-------------------------------|
|  Qt / GTK / Game              |
+---------------+---------------+
                |
                | calls
                v
+-----------------------------------------------+
| Graphics API                                  |
|-----------------------------------------------|
|                                               |
|  +--------------+   +--------------+          |
|  | OpenGL ES    |   | Vulkan       |          |
|  +--------------+   +--------------+          |
|                                               |
|  +--------------+                             |
|  | EGL          |                             |
|  +------+-------+                             |
+---------|-------------------------------------+
          |
          | implemented by
          v
+-------------------------------------------------------+
| Mesa (Userspace)                                      |
|-------------------------------------------------------|
|                                                       |
|  +--------------+     +---------------------------+   |
|  | Gallium3D    |     | Panfrost Driver           |   |
|  +--------------+     +---------------------------+   |
|                                                       |
|  +--------------+                                     |
|  | GBM          |                                     |
|  +------+-------+                                     |
+---------|---------------------------------------------+
          |
          | ioctl
          v
+-----------------------------------------------+
| Kernel                                        |
|-----------------------------------------------|
|                                               |
|  +--------------+   +----------------------+  |
|  | DRM Core     |   | panfrost.ko          |  |
|  +--------------+   +----------------------+  |
|                                               |
|  +--------------+                             |
|  | sun4i-drm.ko |                             |
|  +------+-------+                             |
+---------|-------------------------------------+
          |
          | registers / controls
          v
+-------------------------------------------------------+
| Hardware                                              |
|-------------------------------------------------------|
|                                                       |
|  +--------------+     +---------------------------+   |
|  | Mali-G31 GPU |     | Display Engine            |   |
|  +--------------+     +---------------------------+   |
|                                                       |
|  +--------------+                                     |
|  | HDMI PHY     |                                     |
|  +--------------+                                     |
+-------------------------------------------------------+

Note:
  Panfrost is the open-source Mali driver in Mesa.
```

### Layer summary

```text
Application:
  - Qt, GTK, games

Graphics API:
  - OpenGL ES
  - Vulkan
  - EGL

Mesa:
  - Gallium3D
  - Panfrost
  - GBM

Kernel:
  - DRM core
  - panfrost.ko
  - sun4i-drm.ko

Hardware:
  - Mali-G31 GPU
  - Display Engine
  - HDMI PHY
```

---

## 5.3 GStreamer Video Pipeline

### Original diagram meaning

This diagram shows a GStreamer pipeline using hardware V4L2 decode and DRM/KMS display scanout.

### Text diagram

```text
                         GSTREAMER VIDEO PIPELINE
                         ========================

+--------------------------------+
| GStreamer Pipeline             |
|--------------------------------|
|                                |
|  +--------------------------+  |
|  | filesrc                  |  |
|  +------------+-------------+  |
|               | MP4 container |
|               v               |
|  +--------------------------+  |
|  | qtdemux                  |  |
|  +------------+-------------+  |
|               | H.264 NALs    |
|               v               |
|  +--------------------------+  |
|  | h264parse                |  |
|  +------------+-------------+  |
|               | Parsed stream |
|               v               |
|  +--------------------------+  |
|  | v4l2h264dec              |  |
|  +------------+-------------+  |
|          |    | dma-buf frames |
|          |    v               |
|          | +----------------+ |
|          | | kmssink        | |
|          | +-------+--------+ |
|          |         | Atomic commit
+----------|---------|----------+
           |         |
           | ioctl   v
           v
+--------------------------------+
| Kernel                         |
|--------------------------------|
|                                |
| +------------+  +------------+ |
| | DRM/KMS    |  | V4L2 Layer | |
| +------+-----+  +-----+------+ |
|        |              |        |
|        |              v        |
|        |       +------------+  |
|        +------>| Cedrus     |  |
|                | Driver     |  |
|                +-----+------+  |
+----------------------|---------+
                       |
                       v
+--------------------------------+
| Hardware                       |
|--------------------------------|
| +------------+  +------------+ |
| | VPU        |  | Display    | |
| |            |  | Engine     | |
| +------------+  +------------+ |
+--------------------------------+

Data/control paths:
  filesrc -> qtdemux -> h264parse -> v4l2h264dec -> kmssink
  v4l2h264dec -> ioctl -> V4L2 Layer -> Cedrus Driver -> VPU
  kmssink -> Atomic commit -> DRM/KMS -> Display Engine
  decoded dma-buf frames -> Display Engine scanout
```

### Example pipeline

```bash
gst-launch-1.0 filesrc location=video.mp4 ! \
  qtdemux ! h264parse ! \
  v4l2h264dec ! \
  kmssink
```

---

## 5.4 V4L2 Stateless Decode Flow

### Original diagram meaning

This sequence shows userspace parsing H.264 bitstream and passing controls, output buffers, and capture buffers to V4L2/Cedrus hardware.

### Text diagram

```text
                         V4L2 STATELESS DECODE FLOW
                         ==========================

Participants:
  Userspace(FFmpeg) | V4L2 Core | Cedrus Driver | VPU Hardware


Userspace           V4L2 Core           Cedrus Driver          VPU Hardware
   |                    |                     |                     |
   | Parse H.264 bitstream                    |                     |
   |<-------------------|                     |                     |
   |                    |                     |                     |
   | Set controls (SPS, PPS, Slice)           |                     |
   |------------------->|                     |                     |
   |                    |                     |                     |
   | QBUF OUTPUT (bitstream)                  |                     |
   |------------------->|                     |                     |
   |                    |                     |                     |
   | QBUF CAPTURE (frame buffer)              |                     |
   |------------------->|                     |                     |
   |                    |                     |                     |
   | STREAMON           |                     |                     |
   |------------------->|                     |                     |
   |                    | device_run()        |                     |
   |                    |-------------------->|                     |
   |                    |                     | Program registers   |
   |                    |                     |-------------------->|
   |                    |                     | Start decode        |
   |                    |                     |-------------------->|
   |                    |                     |                     |
   |                    |                     | IRQ complete        |
   |                    |                     |<--------------------|
   |                    | job_finish()        |                     |
   |                    |<--------------------|                     |
   | DQBUF(decoded frame)                     |                     |
   |<-------------------|                     |                     |
   |                    |                     |                     |
```

### Decode responsibilities

```text
Userspace:
  - Parses H.264 bitstream
  - Extracts SPS/PPS/slice parameters
  - Queues bitstream and capture buffers

V4L2 Core:
  - Handles standard V4L2 queue operations
  - Connects userspace buffers to driver callbacks

Cedrus Driver:
  - Programs VPU registers
  - Starts decode
  - Handles IRQ
  - Marks buffer complete

VPU Hardware:
  - Decodes bitstream
  - Writes decoded frame to destination buffer
```

---

## 5.5 Wayland Compositor Flow

### Original diagram meaning

This sequence shows a client rendering into a buffer, attaching/committing it to Weston, and Weston submitting an atomic commit through DRM/KMS to the display.

### Text diagram

```text
                         WAYLAND COMPOSITOR FLOW
                         =======================

Participants:
  Client App | Compositor(Weston) | DRM/KMS | Display


Client App             Compositor(Weston)          DRM/KMS            Display
    |                         |                       |                  |
    | Render to buffer        |                       |                  |
    |<------------------------|                       |                  |
    |                         |                       |                  |
    | wl_surface.attach(buffer)                       |                  |
    |------------------------>|                       |                  |
    |                         |                       |                  |
    | wl_surface.commit()     |                       |                  |
    |------------------------>|                       |                  |
    |                         |                       |                  |
    |                         | Compose all surfaces  |                  |
    |                         |<--------------------  |                  |
    |                         |                       |                  |
    |                         | Atomic commit         |                  |
    |                         |---------------------->|                  |
    |                         |                       | Page flip        |
    |                         |                       |----------------->|
    |                         |                       |                  |
    |                         | VSync                 |                  |
    |                         |<------------------------------------------|
    | frame callback          |                       |                  |
    |<------------------------|                       |                  |
    |                         |                       |                  |
```

### Frame budget

```text
60 FPS = 16.67 ms per frame

Within each frame:
  - Receive buffers
  - Composite
  - Submit to DRM
  - Wait for VSync
  - Send frame callback
```

---

# 6. IVI System Architecture

### Original diagram meaning

This diagram shows a simplified IVI software architecture:

- Applications include media player, navigation, and settings.
- Middleware includes D-Bus, GStreamer, and systemd.
- Graphics includes Mesa/Panfrost and Weston.
- Kernel includes DRM/KMS, V4L2, and input.
- Hardware includes GPU, VPU, and touch.

### Text diagram

```text
                         IVI SYSTEM ARCHITECTURE
                         =======================

+-----------------------------------------+      +------------------------------------------+
| Middleware                              |      | Applications                             |
|-----------------------------------------|      |------------------------------------------|
|                                         |      |                                          |
|  +--------------+   +---------------+   |      |  +--------------+    +----------------+  |
|  | D-Bus        |   | GStreamer     |   |<---->|  | Media Player |    | Navigation     |  |
|  +--------------+   +---------------+   |      |  +--------------+    +----------------+  |
|                                         |      |                                          |
|  +--------------+                       |      |  +--------------+                        |
|  | Systemd      |                       |      |  | Settings     |                        |
|  +--------------+                       |      |  +--------------+                        |
|                                         |      |                                          |
+-------------------+---------------------+      +-------------------+----------------------+
                    |                                            |
                    |                                            |
                    v                                            v
              +---------------------------------------------------------+
              | Graphics                                                |
              |---------------------------------------------------------|
              |                                                         |
              |  +-------------------+        +----------------------+  |
              |  | Mesa / Panfrost   |        | Weston               |  |
              |  +-------------------+        +----------------------+  |
              |                                                         |
              +-----------------------------+---------------------------+
                                            |
                                            v
                         +--------------------------------------+
                         | Kernel                               |
                         |--------------------------------------|
                         |                                      |
                         |  +------------+     +-------------+  |
                         |  | DRM/KMS    |     | V4L2        |  |
                         |  +------------+     +-------------+  |
                         |                                      |
                         |  +------------+                      |
                         |  | Input      |                      |
                         |  +------------+                      |
                         |                                      |
                         +------------------+-------------------+
                                            |
                                            v
                         +--------------------------------------+
                         | Hardware                             |
                         |--------------------------------------|
                         |                                      |
                         |  +------------+     +-------------+  |
                         |  | GPU        |     | VPU         |  |
                         |  +------------+     +-------------+  |
                         |                                      |
                         |  +------------+                      |
                         |  | Touch      |                      |
                         |  +------------+                      |
                         |                                      |
                         +--------------------------------------+
```

### Layer responsibilities

```text
Applications:
  - Media Player
  - Navigation
  - Settings

Middleware:
  - D-Bus for IPC
  - GStreamer for multimedia pipeline
  - Systemd for service management

Graphics:
  - Weston compositor
  - Mesa/Panfrost GPU userspace

Kernel:
  - DRM/KMS display
  - V4L2 video decode
  - Input subsystem

Hardware:
  - GPU
  - VPU
  - Touch controller
```

---

# Summary

This Markdown file converts the uploaded diagrams into editable text-based diagrams. The diagrams cover:

```text
OTA and Secure Update:
  - A/B partitioning
  - dm-verity hash tree
  - CI/CD board farm

AMP and RTOS:
  - Memory layout
  - SMP vs AMP
  - remoteproc boot
  - rpmsg IPC

Real-Time Linux:
  - CPU isolation
  - RT task periodic loop

Kernel and Boot:
  - start_kernel() to userspace
  - SD card image layout
  - defense in depth

Graphics and Multimedia:
  - DRM/KMS object hierarchy
  - graphics software stack
  - GStreamer video pipeline
  - V4L2 stateless decode
  - Wayland compositor

IVI:
  - Full IVI system architecture
```
