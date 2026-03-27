# Concept Breakdown

## Lecture framing

- Title: Bài 2.2: Initramfs & Switch_root
- Confidence: low
- Matched LKMPG topic areas: no_close_match

## Primary lecture headings

- Mục tiêu Bài học
- Phần 1: Initramfs là gì?
- 1.1 Định nghĩa
- 1.2 Initramfs vs Initrd
- 1.3 Khi nào cần Initramfs?
- Phần 2: Cấu trúc Initramfs
- 2.1 Cấu trúc thư mục
- 2.2 Script /init
- Phần 3: Switch_root Chi tiết
- 3.1 switch_root làm gì?

## LKMPG concept anchors

- No close LKMPG example exists for initramfs init scripts, BusyBox rootfs assembly, or switch_root transitions.

## Why the concept matters

The lecture is useful because it explains a systems boundary: how hardware description, scheduling, synchronization, or boot-time structure becomes actual kernel behavior. The LKMPG mapping is valuable when it gives you a concrete callback path, registration step, or interface file to inspect. When there is no close code match, that absence is itself actionable and should guide follow-up reading.
