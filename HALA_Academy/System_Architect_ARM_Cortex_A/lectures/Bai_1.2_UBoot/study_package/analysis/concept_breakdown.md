# Concept Breakdown

## Lecture framing

- Title: Bài 1.2: U-Boot (SPL + Proper)
- Confidence: low
- Matched LKMPG topic areas: no_close_match

## Primary lecture headings

- Mục tiêu Bài học
- Phần 1: Kiến trúc U-Boot
- 1.1 Tại sao cần 2 giai đoạn?
- 1.2 So sánh SPL vs Proper
- Phần 2: U-Boot SPL Chi tiết
- 2.1 Nhiệm vụ chính của SPL
- 2.2 DRAM Initialization
- Phần 3: Build U-Boot
- 3.1 Chuẩn bị
- 3.2 Configure và Build

## LKMPG concept anchors

- No close LKMPG example exists for SPL, U-Boot proper, or bootloader DRAM relocation.

## Why the concept matters

The lecture is useful because it explains a systems boundary: how hardware description, scheduling, synchronization, or boot-time structure becomes actual kernel behavior. The LKMPG mapping is valuable when it gives you a concrete callback path, registration step, or interface file to inspect. When there is no close code match, that absence is itself actionable and should guide follow-up reading.
