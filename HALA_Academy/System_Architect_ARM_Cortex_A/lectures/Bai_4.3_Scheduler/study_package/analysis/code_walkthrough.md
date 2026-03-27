# Code Walkthrough

## sched

- Source files: sched.c
- Type: kernel module
- Topic group: GPIO / IRQ / workqueue / tasklet examples
- Runnable status: directly_runnable
- Walkthrough:
  The example is useful because it keeps the control flow small enough to inspect linearly. Start at module init, identify the objects registered with the kernel, then trace the callback path that exposes the lecture concept. Finish by reading cleanup to understand object lifetime and teardown guarantees.
- Lecture relation:
  Provides a concrete queued-work path for explaining asynchronous scheduler-visible tasks.

## completions

- Source files: completions.c
- Type: kernel module
- Topic group: synchronization examples
- Runnable status: directly_runnable
- Walkthrough:
  The example is useful because it keeps the control flow small enough to inspect linearly. Start at module init, identify the objects registered with the kernel, then trace the callback path that exposes the lecture concept. Finish by reading cleanup to understand object lifetime and teardown guarantees.
- Lecture relation:
  Shows ordering constraints across kthreads, which helps explain latency-sensitive coordination.

## example_tasklet

- Source files: example_tasklet.c
- Type: kernel module
- Topic group: GPIO / IRQ / workqueue / tasklet examples
- Runnable status: directly_runnable
- Walkthrough:
  The example is useful because it keeps the control flow small enough to inspect linearly. Start at module init, identify the objects registered with the kernel, then trace the callback path that exposes the lecture concept. Finish by reading cleanup to understand object lifetime and teardown guarantees.
- Lecture relation:
  Illustrates old-style deferred execution, useful for contrasting latency trade-offs.
