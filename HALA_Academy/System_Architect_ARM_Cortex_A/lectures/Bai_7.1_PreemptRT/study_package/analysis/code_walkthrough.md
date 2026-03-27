# Code Walkthrough

## example_mutex

- Source files: example_mutex.c
- Type: kernel module
- Topic group: synchronization examples
- Runnable status: directly_runnable
- Walkthrough:
  The example is useful because it keeps the control flow small enough to inspect linearly. Start at module init, identify the objects registered with the kernel, then trace the callback path that exposes the lecture concept. Finish by reading cleanup to understand object lifetime and teardown guarantees.
- Lecture relation:
  Provides a small mutex example for discussing why mutexes are favored over raw spinning in many RT paths.

## example_spinlock

- Source files: example_spinlock.c
- Type: kernel module
- Topic group: synchronization examples
- Runnable status: directly_runnable
- Walkthrough:
  The example is useful because it keeps the control flow small enough to inspect linearly. Start at module init, identify the objects registered with the kernel, then trace the callback path that exposes the lecture concept. Finish by reading cleanup to understand object lifetime and teardown guarantees.
- Lecture relation:
  Shows the exact lock family whose semantics change most under RT constraints.

## example_rwlock

- Source files: example_rwlock.c
- Type: kernel module
- Topic group: synchronization examples
- Runnable status: directly_runnable
- Walkthrough:
  The example is useful because it keeps the control flow small enough to inspect linearly. Start at module init, identify the objects registered with the kernel, then trace the callback path that exposes the lecture concept. Finish by reading cleanup to understand object lifetime and teardown guarantees.
- Lecture relation:
  Helps explain how read-mostly locking still carries IRQ-state and contention implications.
