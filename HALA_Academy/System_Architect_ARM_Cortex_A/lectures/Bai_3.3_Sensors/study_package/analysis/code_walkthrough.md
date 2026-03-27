# Code Walkthrough

## dht11

- Source files: dht11.c
- Type: kernel module
- Topic group: GPIO / IRQ / workqueue / tasklet examples
- Runnable status: needs_hardware
- Walkthrough:
  The example is useful because it keeps the control flow small enough to inspect linearly. Start at module init, identify the objects registered with the kernel, then trace the callback path that exposes the lecture concept. Finish by reading cleanup to understand object lifetime and teardown guarantees.
- Lecture relation:
  Shows a concrete sensor-facing module with GPIO timing, device creation, and data exposure.

## devicetree

- Source files: devicetree.c
- Type: kernel module
- Topic group: device-tree related examples
- Runnable status: needs_adaptation
- Walkthrough:
  The example is useful because it keeps the control flow small enough to inspect linearly. Start at module init, identify the objects registered with the kernel, then trace the callback path that exposes the lecture concept. Finish by reading cleanup to understand object lifetime and teardown guarantees.
- Lecture relation:
  Provides the DT property-consumption piece often used by modern sensor drivers.

## hello-sysfs

- Source files: hello-sysfs.c
- Type: kernel module
- Topic group: sysfs examples
- Runnable status: directly_runnable
- Walkthrough:
  The example is useful because it keeps the control flow small enough to inspect linearly. Start at module init, identify the objects registered with the kernel, then trace the callback path that exposes the lecture concept. Finish by reading cleanup to understand object lifetime and teardown guarantees.
- Lecture relation:
  Shows a minimal attribute interface that helps explain why sensor frameworks expose stable userspace views.
