# Code Walkthrough

## example_mutex

- Source files: example_mutex.c
- Type: kernel module
- Topic group: synchronization examples
- Runnable status: directly_runnable
- Walkthrough:
  The example is useful because it keeps the control flow small enough to inspect linearly. Start at module init, identify the objects registered with the kernel, then trace the callback path that exposes the lecture concept. Finish by reading cleanup to understand object lifetime and teardown guarantees.
- Lecture relation:
  Gives a clear mutex-based starting point for explaining why priority inheritance matters.

## completions

- Source files: completions.c
- Type: kernel module
- Topic group: synchronization examples
- Runnable status: directly_runnable
- Walkthrough:
  The example is useful because it keeps the control flow small enough to inspect linearly. Start at module init, identify the objects registered with the kernel, then trace the callback path that exposes the lecture concept. Finish by reading cleanup to understand object lifetime and teardown guarantees.
- Lecture relation:
  Shows an explicit hand-off primitive that fits RT design-pattern discussions.

## example_atomic

- Source files: example_atomic.c
- Type: kernel module
- Topic group: synchronization examples
- Runnable status: directly_runnable
- Walkthrough:
  The example is useful because it keeps the control flow small enough to inspect linearly. Start at module init, identify the objects registered with the kernel, then trace the callback path that exposes the lecture concept. Finish by reading cleanup to understand object lifetime and teardown guarantees.
- Lecture relation:
  Illustrates when atomic operations can replace heavier locking for very small shared-state updates.
