# Concept Breakdown

## Lecture framing

- Title: Bài 2.1: Kernel Build & Device Tree Baseline
- Confidence: medium
- Matched LKMPG topic areas: device-tree related examples, device model, kernel modules

## Primary lecture headings

- Mục tiêu Bài học
- Phần 1: Luồng Khởi động Kernel
- 1.1 Từ U-Boot sang Kernel
- 1.2 Chi tiết start_kernel()
- 1.3 Từ rest_init() đến Userspace
- Phần 2: Device Tree (DTB)
- 2.1 Device Tree là gì?
- 2.2 Cấu trúc cơ bản
- Phần 3: Build Linux Kernel
- 3.1 Chuẩn bị

## LKMPG concept anchors

- `devicetree` maps to **Device Tree matching and DTB-consumed properties** and explains shows how a module reads dt properties and probes through of_match_table..
- `devicemodel` maps to **Platform-driver registration during kernel/runtime bring-up** and explains shows the driver side of platform registration that dt-backed devices ultimately bind to..
- `hello-1` maps to **Post-build kernel module validation workflow** and explains provides the simplest external-module sanity check once kernel headers and build tooling are in place..

## Why the concept matters

The lecture is useful because it explains a systems boundary: how hardware description, scheduling, synchronization, or boot-time structure becomes actual kernel behavior. The LKMPG mapping is valuable when it gives you a concrete callback path, registration step, or interface file to inspect. When there is no close code match, that absence is itself actionable and should guide follow-up reading.
