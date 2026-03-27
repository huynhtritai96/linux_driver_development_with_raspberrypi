# Concept Breakdown

## Lecture framing

- Title: Bài 3.2: Touchscreen Driver (Input Subsystem)
- Confidence: medium
- Matched LKMPG topic areas: input subsystem, device-tree related examples

## Primary lecture headings

- Mục tiêu Bài học
- Phần 1: Linux Input Subsystem
- 1.1 Kiến trúc
- 1.2 Event Types
- 1.3 Touch Events (EV_ABS)
- Phần 2: Driver Structure
- 2.1 IRQ Handler
- 2.2 Probe Function
- Phần 3: Device Tree Binding
- Phần 4: Debug và Test

## LKMPG concept anchors

- `vinput` maps to **Input device registration and event plumbing** and explains shows how an input-oriented module exposes a control path and interacts with the input subsystem..
- `devicetree` maps to **Touch controller DT description and binding** and explains connects lecture dt bindings to an lkmpg module that actually consumes device-tree properties..

## Why the concept matters

The lecture is useful because it explains a systems boundary: how hardware description, scheduling, synchronization, or boot-time structure becomes actual kernel behavior. The LKMPG mapping is valuable when it gives you a concrete callback path, registration step, or interface file to inspect. When there is no close code match, that absence is itself actionable and should guide follow-up reading.
