

```
====================================================================================================
LINUX KERNEL THREADS — SENIOR MENTAL MODEL
====================================================================================================

[ BIG PICTURE ]
----------------------------------------------------------------------------------------------------
Linux system execution entities can be viewed like this:
    CPU Scheduler
        |
        +--> user process thread
        +--> user process thread
        +--> kernel thread
        +--> kernel thread
        +--> interrupt-related work
        +--> many other schedulable tasks

COMMENT:
- The scheduler mostly cares about "who is runnable now?"
- It does NOT deeply care whether the task belongs to user space or kernel space.
- From scheduling perspective, both are tasks competing for CPU time.

====================================================================================================
[ WHAT IS A PROCESS? ]
====================================================================================================
A process is a running instance of a user-space program.

Examples:
    bash
    firefox
    python
    your C application

A process usually owns:
    - virtual address space
    - user stack(s)
    - heap
    - file descriptors
    - credentials / security context
    - one or more threads

Mental picture:
    Process
      |
      +-- address space
      +-- heap
      +-- global variables
      +-- file descriptor table
      +-- thread A
      +-- thread B
      +-- thread C

COMMENT:
- A process is more like a resource container than "just code running".
- Threads execute inside that container.
- So when people say "the process is running", in practice one or more threads are executing.

====================================================================================================
[ WHAT IS A THREAD? ]
====================================================================================================
A thread is the smallest schedulable execution path inside a process.

Threads in the same process share:
    - virtual address space
    - globals
    - heap
    - open files

Each thread has its own:
    - stack
    - CPU register state
    - scheduling state
    - program counter / execution context

Mental picture:
    One Process
      |
      +-- shared address space
      |      +-- global data
      |      +-- heap
      |      +-- file descriptors
      |
      +-- thread 1 (own stack, own CPU context)
      +-- thread 2 (own stack, own CPU context)
      +-- thread 3 (own stack, own CPU context)

COMMENT:
- The easiest mistake is thinking threads are "just functions".
- They are not.
- A thread is an independent execution context.
- Two threads can run the same function but are still separate executions.

====================================================================================================
[ USER-SPACE THREADS vs KERNEL THREADS ]
====================================================================================================

USER-SPACE THREADS
------------------
Created by:
    pthread
    std::thread
    Java/Python/Rust thread runtimes
    etc.

Run in:
    user space

Can directly access:
    - user memory
    - process resources

Cannot directly access:
    - kernel memory
    - hardware registers
    - kernel internal APIs

Need system calls to request kernel services.

KERNEL THREADS
--------------
Created by:
    kthread APIs inside the kernel

Run in:
    kernel space

Can directly access:
    - kernel memory
    - kernel APIs
    - driver data structures
    - hardware-facing kernel mechanisms

Have:
    - no normal user-space address space of their own
    - their own task_struct
    - their own stack and execution context

COMMENT:
The biggest conceptual difference is not “both are threads”.
The biggest difference is:
    user thread -> lives inside a user process context
    kernel thread -> lives entirely inside kernel execution context

So a kernel thread is not "a faster pthread".
It is a kernel-managed execution worker.

====================================================================================================
[ WHERE KERNEL THREADS LIVE ]
====================================================================================================
User space:
    applications
    libraries
    user threads

Kernel space:
    memory manager
    scheduler
    filesystems
    drivers
    kernel threads

Mental picture:
    User Building
        ├── bash
        ├── firefox
        ├── app thread 1
        └── app thread 2

    Kernel Building
        ├── scheduler
        ├── VFS
        ├── drivers
        ├── kworker/...
        ├── kthreadd
        └── your custom kthread

COMMENT:
This "two buildings" picture is very useful.
A kernel thread is a worker that never leaves the kernel building.

====================================================================================================
[ HOW KERNEL THREADS APPEAR IN THE SYSTEM ]
====================================================================================================
Kernel threads:
    - have task_struct
    - are visible to process listing tools
    - usually appear with names in square brackets in many tools

Examples:
    [kthreadd]
    [kworker/0:1]
    [ksoftirqd/0]

Your custom threads may appear with names like:
    kthread_1
    kthread_2

Tools:
    ps -ef
    top

COMMENT:
- Linux uses similar task infrastructure for both user and kernel execution entities.
- That is why kernel threads are visible in process tools.
- They are not "hidden magic threads"; they are real scheduled tasks.

====================================================================================================
[ WHY DRIVERS USE KERNEL THREADS ]
====================================================================================================
Drivers use kernel threads when they need long-lived background execution such as:
    - periodic polling
    - waiting/sleeping loops
    - blocking operations
    - monitoring hardware state
    - deferred but persistent work
    - communication loops with firmware/devices

Examples:
    sensor polling
    DMA management loop
    firmware mailbox loop
    health monitoring loop
    retry/recovery logic

COMMENT:
Use a kernel thread when the driver needs a worker that:
    - can sleep
    - can loop
    - can block
    - can run independently over time

A kernel thread is NOT always the best tool.
But it is a very natural fit for persistent background driver logic.

====================================================================================================
[ YOUR EXAMPLE — WHAT OBJECTS EXIST ]
====================================================================================================
Global state in your module:
    static struct task_struct *thread_1;
    static struct task_struct *thread_2;

    static int t1 = 1;
    static int t2 = 2;

Meaning:
    thread_1 / thread_2 = handles to the kernel thread tasks created by kthread_run()
    t1 / t2 = simple pieces of per-thread input data passed to the thread function

COMMENT:
- The task_struct pointer is not "the thread code".
- It is the kernel's task object representing that running thread instance.
- You use it later to stop/manage the thread.

====================================================================================================
[ THREAD CREATION FLOW ]
====================================================================================================

In module init:
    thread_1 = kthread_run(thread_function, &t1, "kthread_1");
    thread_2 = kthread_run(thread_function, &t2, "kthread_2");

Meaning of kthread_run():
    1. create a kernel thread task
    2. associate it with the given entry function
    3. pass the given data pointer to that function
    4. wake it up so it starts running

Return value:
    success -> struct task_struct *
    failure -> error pointer

So you must check:
    IS_ERR(thread_1)
    IS_ERR(thread_2)

COMMENT:
`kthread_run()` is conceptually:
    create + start

It is a convenience helper. That is why it feels very direct.

====================================================================================================
[ THREAD ENTRY FUNCTION ]
====================================================================================================

Your thread entry point: static int thread_function(void *data)

Input:
    data = whatever pointer you passed from kthread_run()

In your code:
    data -> &t1 or &t2

Then inside:
    int thread_id = *(int *)data;

Meaning:
    same function code
    but each thread instance receives a different input argument

COMMENT:
This is a very important lesson:
You do NOT need different functions to create different threads.

You can do:
    same function
        +
    different input data
        =
    multiple independent thread instances

This is exactly how many real driver workers are written.

====================================================================================================
[ SAME FUNCTION, DIFFERENT THREAD INSTANCES ]
====================================================================================================

Creation:
    kthread_run(thread_function, &t1, "kthread_1")
    kthread_run(thread_function, &t2, "kthread_2")

Runtime view:
    thread instance A : executes thread_function(data=&t1)
    thread instance B : executes thread_function(data=&t2)

Even though function code is the same:
    - each thread has its own stack
    - each thread has its own CPU context
    - each thread has its own timing
    - each thread has its own local variables during execution

COMMENT: This is one of the easiest beginner confusions.

People often think: "same function" means "shared execution"

No.
It means: same code text but separate runtime execution contexts

So your `counter` local variable is independent in each thread because each thread has its own stack frame.

====================================================================================================
[ MAIN THREAD LOOP ]
====================================================================================================

Your loop:
    while (!kthread_should_stop()) {
        pr_info(...);
        ssleep(thread_id);
    }

Meaning:
    keep running until someone requests stop

`kthread_should_stop()` : returns true after another context calls `kthread_stop()` for this task

`ssleep(thread_id)`
    thread 1 sleeps 1 second
    thread 2 sleeps 2 seconds

So runtime behavior becomes:
    thread 1: wake -> print -> sleep 1s -> wake -> print -> sleep 1s ...
    thread 2: wake -> print -> sleep 2s -> wake -> print -> sleep 2s ...

COMMENT:
This is the standard kernel-thread pattern:

    setup
    while (!should_stop)
        do work
        maybe sleep/block
    cleanup
    return

If you remember only one kthread structure, remember this one.

====================================================================================================
[ WHY THE THREAD DOES NOT EAT 100% CPU ]
====================================================================================================
Because it calls:
    ssleep(...)

Without sleeping/blocking, the loop would become:

    while (!kthread_should_stop()) {
        print / spin forever
    }

That would busy-loop and burn CPU.

COMMENT:
Kernel threads are not special here.
Just like user threads, a loop with no blocking/sleep can consume an entire CPU core.

So in background worker threads: always think carefully about pacing

Common pacing methods:
    ssleep()
    msleep()
    wait_event()
    schedule_timeout()
    blocking on queues/events

====================================================================================================
[ STOP FLOW ]
====================================================================================================

In module exit:

    if (thread_1)
        kthread_stop(thread_1);

    if (thread_2)
        kthread_stop(thread_2);

Meaning of kthread_stop():
    - request the target thread to stop
    - wake it if needed
    - wait until the thread function exits
    - return the thread function's return value

So stop flow is:
    module exit
        |
        v
    kthread_stop(thread_1)
        |
        v
    thread_1 sees kthread_should_stop() == true
        |
        v
    exits loop
        |
        v
    returns from thread_function
        |
        v
    kthread_stop() completes

COMMENT:
This is why module unload may take a little time: the caller waits for the thread to actually finish.

So `kthread_stop()` is not just: "send signal and forget"
It is more like: "request stop and join the thread"

====================================================================================================
[ WHY UNLOAD WAITS A LITTLE ]
====================================================================================================

Your thread may currently be inside:
    ssleep(1)
or
    ssleep(2)

So when `kthread_stop()` is called:
    the kernel cannot instantly teleport the thread past the sleep.
    it must let the thread wake and then observe stop state.

That is why unload may feel delayed.

COMMENT:
This is a very important real-world observation: stopping a thread is cooperative.

The thread must eventually return from its loop.
So your loop structure determines how quickly shutdown happens.

If you need fast shutdown: avoid very long uninterruptible waits inside the loop.

====================================================================================================
[ ERROR HANDLING IN INIT ]
====================================================================================================

Creation order:
    create thread_1
    create thread_2

If thread_2 creation fails:
    thread_1 is already running

So code does:

    kthread_stop(thread_1);
    return PTR_ERR(thread_2);

Meaning:
    do not leave a background thread alive if module init fails halfway

COMMENT:
This is classic kernel resource-lifetime discipline:
    acquire resources in order
    if later acquisition fails,
    unwind earlier acquisitions before returning error

Threads are resources too.

====================================================================================================
[ SCHEDULER VIEW ]
====================================================================================================

Scheduler does not think: "this is a magical kernel helper"
Scheduler thinks: "this is another runnable task"

So internally:
    CPU Scheduler
         |
         +-- user thread A
         +-- user thread B
         +-- kernel thread 1
         +-- kernel thread 2
         +-- other tasks

Properties:
    - they compete for CPU
    - they can sleep
    - they can wake
    - they can be preempted

COMMENT:
This is one of the deepest points to understand:
kernel threads are not outside the scheduler.
They are part of the same scheduling world.

====================================================================================================
[ WHAT IS PRIVATE TO EACH THREAD IN YOUR EXAMPLE ]
====================================================================================================

Shared module globals:
    module_name
    thread_1 pointer
    thread_2 pointer
    t1
    t2

Per-thread private execution state:
    thread_id
    counter
    stack
    CPU registers
    sleep timing
    current loop iteration

COMMENT:
This explains why:
    both threads use the same function
but
    their counters evolve independently

Because `counter` is a local variable on each thread's own stack.

This is a foundational concurrency concept.

====================================================================================================
[ COMPLETE LIFECYCLE OF YOUR MODULE ]
====================================================================================================

MODULE LOAD
    |
    +--> create thread_1
    |
    +--> create thread_2
    |
    +--> both start running

RUNTIME
    |
    +--> thread_1 prints every 1 second
    |
    +--> thread_2 prints every 2 seconds

MODULE UNLOAD
    |
    +--> stop thread_1
    |
    +--> stop thread_2
    |
    +--> both loops exit
    |
    +--> module unload completes

COMMENT:
If you can replay this lifecycle in your head, you already understand the example well.

====================================================================================================
[ BEST SIMPLE MENTAL MODEL ]
====================================================================================================

Process = resource container in user space
Thread = execution worker inside that container
Kernel thread = execution worker that lives entirely in kernel space
kthread_run() = create + start worker
thread_function() = worker entry point
while (!kthread_should_stop()) = cooperative run loop
kthread_stop() = request stop + wait for worker to exit

====================================================================================================
[ SENIOR COMMENTS TO HELP YOU REMEMBER ]
====================================================================================================

1. **A kernel thread is still a scheduled task**
It is not “special background magic”.
It is a normal schedulable kernel task.

2. **Same function does not mean same thread**
Two calls to `kthread_run()` create two separate execution instances.

3. **The loop is cooperative**
The kernel thread must check `kthread_should_stop()` and exit cleanly.

4. **Sleep controls pacing**
Without sleep or blocking, the thread can busy-spin and waste CPU.

5. **Stop is part of resource cleanup**
If your module starts threads, your module must stop them safely.

6. **Private local variables stay private because stacks are separate**
That is why each thread gets its own `counter`.

====================================================================================================

```