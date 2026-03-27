# Concept Breakdown

## Lecture framing

- Title: Bài 6.1: Advanced V4L2 Stateless Codec Architecture
- Confidence: low
- Matched LKMPG topic areas: no_close_match

## Primary lecture headings

- Mục tiêu Bài học
- 1.1 Stateful vs Stateless Architecture
- 2.1 Vấn đề của frame-based decoding
- 2.2 Giải pháp Media Request
- 2.3 Workflow Chi tiết
- 3.1 Register Map & Memory IO
- 3.2 Slice Decoding Flow
- 4.1 Kiểm tra Kernel Support
- 4.2 Thiết lập FFmpeg cho Request API

## LKMPG concept anchors

- No close LKMPG example exists for stateless codec drivers, Media Request API, or Cedrus-like VPU flows.

## Why the concept matters

The lecture is useful because it explains a systems boundary: how hardware description, scheduling, synchronization, or boot-time structure becomes actual kernel behavior. The LKMPG mapping is valuable when it gives you a concrete callback path, registration step, or interface file to inspect. When there is no close code match, that absence is itself actionable and should guide follow-up reading.
