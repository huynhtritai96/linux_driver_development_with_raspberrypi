# Code Walkthrough

## vinput

- Source files: vinput.c, vinput.h
- Type: kernel module
- Topic group: input subsystem
- Runnable status: buildable_but_interactive
- Walkthrough:
  The example is useful because it keeps the control flow small enough to inspect linearly. Start at module init, identify the objects registered with the kernel, then trace the callback path that exposes the lecture concept. Finish by reading cleanup to understand object lifetime and teardown guarantees.
- Lecture relation:
  Shows how an input-oriented module exposes a control path and interacts with the input subsystem.

## devicetree

- Source files: devicetree.c
- Type: kernel module
- Topic group: device-tree related examples
- Runnable status: needs_adaptation
- Walkthrough:
  The example is useful because it keeps the control flow small enough to inspect linearly. Start at module init, identify the objects registered with the kernel, then trace the callback path that exposes the lecture concept. Finish by reading cleanup to understand object lifetime and teardown guarantees.
- Lecture relation:
  Connects lecture DT bindings to an LKMPG module that actually consumes device-tree properties.
