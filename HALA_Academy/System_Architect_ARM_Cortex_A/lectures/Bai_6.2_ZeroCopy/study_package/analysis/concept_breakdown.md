# Concept Breakdown

## Lecture framing

- Title: Bài 6.2: Advanced Zero-Copy Pipeline & AFBC
- Confidence: low
- Matched LKMPG topic areas: no_close_match

## Primary lecture headings

- Mục tiêu Bài học
- 1.1 Tại sao cần Zero-Copy?
- 1.2 DMA-BUF Workflow
- 2.1 Giới thiệu AFBC
- 2.2 Cơ chế hoạt động
- 2.3 Sử dụng DRM Modifiers
- 3.1 Implicit vs Explicit Sync
- 3.2 Lợi ích
- 4.1 Kiểm tra Zero-copy với GStreamer
- 4.2 Monitor Bandwidth

## LKMPG concept anchors

- No close LKMPG example exists for zero-copy VPU-to-display paths, AFBC, or dma_fence coordination.

## Why the concept matters

The lecture is useful because it explains a systems boundary: how hardware description, scheduling, synchronization, or boot-time structure becomes actual kernel behavior. The LKMPG mapping is valuable when it gives you a concrete callback path, registration step, or interface file to inspect. When there is no close code match, that absence is itself actionable and should guide follow-up reading.
