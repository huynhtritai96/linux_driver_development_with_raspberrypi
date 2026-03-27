# Concept Breakdown

## Lecture framing

- Title: Bài 3.3: Sensor Driver (Regmap & IIO)
- Confidence: medium
- Matched LKMPG topic areas: GPIO / IRQ / workqueue / tasklet examples, device-tree related examples, sysfs examples

## Primary lecture headings

- Mục tiêu Bài học
- Phần 1: IIO Subsystem
- 1.1 IIO là gì?
- 1.2 IIO Channel Types
- Phần 2: Regmap API
- 2.1 Tại sao dùng Regmap?
- 2.2 Regmap Config
- 2.3 Regmap Operations
- Phần 3: IIO Driver Structure
- 3.1 IIO Channel Definition

## LKMPG concept anchors

- `dht11` maps to **Sensor-driver structure and hardware interaction** and explains shows a concrete sensor-facing module with gpio timing, device creation, and data exposure..
- `devicetree` maps to **Sensor instantiation from DT** and explains provides the dt property-consumption piece often used by modern sensor drivers..
- `hello-sysfs` maps to **Userspace-visible configuration and telemetry surfaces** and explains shows a minimal attribute interface that helps explain why sensor frameworks expose stable userspace views..

## Why the concept matters

The lecture is useful because it explains a systems boundary: how hardware description, scheduling, synchronization, or boot-time structure becomes actual kernel behavior. The LKMPG mapping is valuable when it gives you a concrete callback path, registration step, or interface file to inspect. When there is no close code match, that absence is itself actionable and should guide follow-up reading.
