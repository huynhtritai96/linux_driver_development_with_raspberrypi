# Concept Breakdown: Device Tree

## Why this concept matters
The Device Tree chapter captures core Linux kernel module engineering patterns that appear in production subsystems: lifecycle control, kernel/user interfaces, synchronization, and failure-aware cleanup.

## Core concepts in this unit
- of_property_read_* APIs
- of_match_table and MODULE_DEVICE_TABLE
- devm-managed allocations in probe
- overlay compile/apply workflow

## Kernel space vs user space implications
- Kernel-space code must validate and bound user input before acting on it.
- User-space interactions are transport mechanisms; kernel code remains authoritative for state and lifetime.
- Privileged operations (module insertion/removal, device node writes, sysfs writes) require explicit access control and careful cleanup.

## Lifecycle and concurrency concerns
- Init paths should register resources in strict order and unwind precisely on failure.
- Exit paths must reverse allocations and prevent use-after-free by ensuring callbacks/work are drained.
- Synchronization choices must match context (sleepable vs atomic/IRQ).
