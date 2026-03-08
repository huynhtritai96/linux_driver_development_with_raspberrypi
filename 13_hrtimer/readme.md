# GPIO Interrupt Handling

### Video :
[![Youtube Video](https://img.youtube.com/vi/tVhyrcJaXkI/0.jpg)](https://www.youtube.com/watch?v=tVhyrcJaXkI)


```
====================================================================================================
LINUX HIGH-RESOLUTION TIMERS (hrtimer) — SENIOR MENTAL MODEL
====================================================================================================

[ BIG PICTURE ]
----------------------------------------------------------------------------------------------------
An hrtimer is NOT:
    - a userspace sleep
    - a busy wait
    - a simple delay loop
    - "just another thread"

An hrtimer IS:
    - a kernel timer object
    - scheduled by the kernel against a precise clock source
    - used when timing accuracy matters more than coarse timeout efficiency

Mental model:
    "Please call my callback at this precise future time."

Or in periodic mode:
    "Please call my callback repeatedly at this precise interval."

COMMENT:
A lot of beginners think timers are "something that pauses code".
That is not the right model.

A timer does NOT pause your function and resume it later.
Instead, it registers future work with the kernel.
When the expiration time arrives, the kernel invokes your callback.

====================================================================================================
WHY hrtimer EXISTS
====================================================================================================

LINUX HAS TWO DIFFERENT TIMER WORLDS
----------------------------------------------------------------------------------------------------

1. LOW-RESOLUTION TIMERS
------------------------
Based on:
    jiffies
    kernel tick frequency (HZ)

Example:
    if HZ = 250
    then 1 tick = 1 / 250 second = 4 ms

Meaning:
    ordinary timers naturally align to coarse kernel ticks

Good for:
    - timeouts
    - debounce delays
    - watchdog-like checks
    - many timers where exact precision is not critical

Mental model:
    "Wake me roughly around that time."

2. HIGH-RESOLUTION TIMERS
-------------------------
Based on:
    high-resolution kernel timing framework

Not limited to:
    next jiffy boundary

Good for:
    - short precise intervals
    - sub-jiffy timing
    - periodic precise callbacks
    - timing measurement and control logic

Mental model:
    "Wake me as close as possible to this exact time."

COMMENT:
The key difference is not just "one is fast, one is slow".

The deeper difference is:

    low-res timer:
        optimized for efficiency and scale

    hrtimer:
        optimized for timing precision

So the real question is:
    do I care more about exact time,
    or do I care more about cheap coarse timeouts?

====================================================================================================
WHY LOW-RES TIMERS CANNOT REPLACE hrtimers
====================================================================================================

Suppose:
    HZ = 250
then:
    one kernel tick = 4 ms

Now imagine you want:
    1 ms periodic callback

A jiffies-based timer cannot represent 1 ms accurately because:
    1 ms < 1 tick

So low-res timer world says:
    "I can only naturally reason in tick-sized chunks"

But hrtimer says:
    "I can reason in much finer units"

COMMENT:
This is the most practical reason hrtimer exists.
If your desired interval is smaller than one system tick,
ordinary timers are the wrong tool.

====================================================================================================
WHAT THIS MODULE OWNS
====================================================================================================

Global state:
----------------------------------------------------------------------------------------------------
    static struct hrtimer my_hrtimer;
    static ktime_t period;
    static u64 timestamps[MAX_SAMPLE];
    static int index;
    static u64 start_time;

Meaning:
----------------------------------------------------------------------------------------------------
my_hrtimer
    = the kernel timer object itself

period
    = desired timer interval
    = how far into the future each expiration should be

timestamps[]
    = records actual callback execution times

index
    = how many timestamps were recorded

start_time
    = initial reference point for one-shot timing measurement

COMMENT:
Think of this module as owning two things:

    1. one timer engine      -> my_hrtimer
    2. one measurement log   -> timestamps[]

So the example is not only "using a timer".
It is also "measuring how accurate the timer really was".

====================================================================================================
TIME REPRESENTATION — HOW THE KERNEL THINKS ABOUT TIME HERE
====================================================================================================

Two important representations appear:
----------------------------------------------------------------------------------------------------

1. ktime_t
----------
Used by:
    hrtimer APIs

Purpose:
    represent time intervals / expiration times in kernel timer framework

Mental model:
    "time object used for scheduling"

2. u64 nanoseconds
------------------
Used by:
    ktime_get_ns()

Purpose:
    easy measurement, arithmetic, and printing

Mental model:
    "raw timestamp value for observation"

COMMENT:
A very useful practical rule:

    ktime_t
        = use when interacting with timer APIs

    u64 nanoseconds
        = use when measuring and printing elapsed time

So:
    schedule with ktime_t
    analyze with u64

====================================================================================================
PERIOD CREATION
====================================================================================================

Code:
----------------------------------------------------------------------------------------------------
    period = ktime_set(0, 1000000);   // 1 ms

Meaning:
----------------------------------------------------------------------------------------------------
seconds     = 0
nanoseconds = 1,000,000

So:
    period = 1 ms

Examples:
----------------------------------------------------------------------------------------------------
1 second:
    ktime_set(1, 0)

20 ms:
    ktime_set(0, 20 * 1000 * 1000)

500 us:
    ktime_set(0, 500 * 1000)

COMMENT:
This is one of the easiest places to build a strong intuition.

Read it as:

    ktime_set(seconds, nanoseconds)

So:
    ktime_set(0, 1000000)
means:
    no whole seconds,
    plus one million nanoseconds

====================================================================================================
MODULE INIT — COMPLETE TIMER STARTUP FLOW
====================================================================================================

insmod my_hrtimer.ko
    |
    v
my_init()
    |
    +--> hrtimer_init(&my_hrtimer, CLOCK_MONOTONIC, HRTIMER_MODE_REL)
    |
    +--> my_hrtimer.function = hrtimer_callback
    |
    +--> start_time = ktime_get_ns()
    |
    +--> period = ktime_set(...)
    |
    +--> hrtimer_start(&my_hrtimer, period, HRTIMER_MODE_REL)
    |
    +--> print "hr-timer started"

COMMENT:
This startup flow has five different responsibilities:

    1. create timer object
    2. define clock base
    3. attach callback
    4. define interval
    5. arm the timer

That is the clean mental breakdown.

====================================================================================================
STEP 1 — hrtimer_init()
====================================================================================================

Code:
----------------------------------------------------------------------------------------------------
    hrtimer_init(&my_hrtimer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);

What it does:
----------------------------------------------------------------------------------------------------
Initializes the timer object so the kernel knows:

    - which clock source to use
    - how expiration values should be interpreted

This does NOT start the timer yet.

It only prepares the object.

COMMENT:
A very common beginner mistake:
    thinking init means start

No.

`hrtimer_init()` only sets up the timer object.
The timer becomes active only after `hrtimer_start()`.

====================================================================================================
CLOCK SOURCE — WHY CLOCK_MONOTONIC IS USED
====================================================================================================

CLOCK_MONOTONIC means:
----------------------------------------------------------------------------------------------------
use a steadily increasing elapsed-time clock

Properties:
    - always moves forward
    - not affected by user changing system date/time
    - not affected by wall-clock corrections the way realtime can be

Why good for interval timers:
----------------------------------------------------------------------------------------------------
If you want:
    "run every 1 ms"
you care about elapsed time,
not about calendar time

So monotonic is the natural clock base.

COMMENT:
This distinction is very important:

    CLOCK_REALTIME
        = "what time is it in the real world?"

    CLOCK_MONOTONIC
        = "how much time has passed steadily?"

Timers for periodic driver logic almost always want the second idea.

====================================================================================================
MODE — WHY HRTIMER_MODE_REL IS USED
====================================================================================================

HRTIMER_MODE_REL means:
----------------------------------------------------------------------------------------------------
the time value passed to start/forward is interpreted as:

    "relative from now"

Example:
    hrtimer_start(timer, period, HRTIMER_MODE_REL)

means:
    "expire after period from current moment"

Not:
    "expire at absolute timestamp X"

COMMENT:
This is very easy to understand if you compare:

    REL mode
        = after 1 ms from now

    ABS mode
        = at exact timestamp T

For repeating intervals, REL mode is often the easier mental model.

====================================================================================================
STEP 2 — ATTACH CALLBACK
====================================================================================================

Code:
----------------------------------------------------------------------------------------------------
    my_hrtimer.function = &hrtimer_callback;

Meaning:
----------------------------------------------------------------------------------------------------
When the timer expires,
the kernel will invoke this callback.

Callback signature:
----------------------------------------------------------------------------------------------------
    static enum hrtimer_restart hrtimer_callback(struct hrtimer *timer)

Important:
----------------------------------------------------------------------------------------------------
The callback does two jobs:

    1. perform timer-related work
    2. tell the kernel whether the timer continues or stops

Return values:
----------------------------------------------------------------------------------------------------
HRTIMER_NORESTART
    = one-shot timer, stop after this callback

HRTIMER_RESTART
    = timer should continue

COMMENT:
This is a subtle but very important design point:
the callback is not only a "work function".
It is also part of the timer lifecycle control.

====================================================================================================
STEP 3 — CAPTURE start_time
====================================================================================================

Code:
----------------------------------------------------------------------------------------------------
    start_time = ktime_get_ns();

Meaning:
----------------------------------------------------------------------------------------------------
store current monotonic time in nanoseconds

Purpose:
----------------------------------------------------------------------------------------------------
used in one-shot version to measure:

    callback_time - start_time

So you can check how close actual expiry was to requested interval.

COMMENT:
This is not required for timer functionality.
It is instrumentation.
It exists only so you can observe accuracy.

That is a good debugging habit:
    measure what the timer really did,
    not what you hope it did.

====================================================================================================
STEP 4 — hrtimer_start()
====================================================================================================

Code:
----------------------------------------------------------------------------------------------------
    hrtimer_start(&my_hrtimer, period, HRTIMER_MODE_REL);

Meaning:
----------------------------------------------------------------------------------------------------
arm the timer and schedule its first expiration

Mental flow:
----------------------------------------------------------------------------------------------------
timer object
    + expiration interval
    + relative mode
        |
        v
kernel inserts timer into its high-resolution scheduling system
        |
        v
callback will be triggered when expiration time arrives

COMMENT:
This is the moment the timer becomes alive.

Before this:
    timer exists, but inactive

After this:
    timer is pending and scheduled

====================================================================================================
ONE-SHOT TIMER MODEL
====================================================================================================

One-shot callback example:
----------------------------------------------------------------------------------------------------
    hrtimer_callback(timer):
        now = ktime_get_ns()
        print(now - start_time)
        return HRTIMER_NORESTART

Execution flow:
----------------------------------------------------------------------------------------------------
module load
    |
    +--> start_time captured
    +--> timer armed for +period
    |
    v
time passes
    |
    v
expiration occurs
    |
    v
callback runs once
    |
    +--> measure elapsed time
    +--> return NORESTART
    |
    v
timer becomes inactive

COMMENT:
A one-shot hrtimer is conceptually just:
    "do this callback once at a precise future time"

That is the simplest way to understand hrtimer first.

====================================================================================================
PERIODIC TIMER MODEL
====================================================================================================

Periodic callback code:
----------------------------------------------------------------------------------------------------
    static enum hrtimer_restart hrtimer_callback(struct hrtimer *timer)
    {
        if (index < MAX_SAMPLE)
            timestamps[index++] = ktime_get_ns();

        hrtimer_forward_now(timer, period);

        return HRTIMER_RESTART;
    }

This is the real heart of the example.

COMMENT:
Periodic mode needs TWO things:

    1. tell kernel timer should continue
           -> return HRTIMER_RESTART

    2. define next expiration time
           -> hrtimer_forward_now(timer, period)

Many beginners remember only the return value.
That is incomplete.

Returning RESTART alone is not enough.
The timer must also be advanced to the next deadline.

====================================================================================================
WHY hrtimer_forward_now() IS NECESSARY
====================================================================================================

Code:
----------------------------------------------------------------------------------------------------
    hrtimer_forward_now(timer, period);

Meaning:
----------------------------------------------------------------------------------------------------
move this timer's next expiration forward by one period
relative to current timer base

Mental model:
----------------------------------------------------------------------------------------------------
first expiration happened
now you must tell kernel:
    "schedule the next one period later"

Without forward_now():
----------------------------------------------------------------------------------------------------
there is no valid next deadline

So no true periodic schedule exists.

Periodic sequence:
----------------------------------------------------------------------------------------------------
t0:
    callback runs
    forward timer by +period
    return RESTART

t1:
    callback runs
    forward timer by +period
    return RESTART

t2:
    callback runs
    forward timer by +period
    return RESTART

COMMENT:
The cleanest way to think about periodic hrtimer is:

    callback
        ->
    record time / do work
        ->
    move next deadline
        ->
    tell kernel to keep going

That is the full loop.

====================================================================================================
TIMESTAMP SAMPLING — WHY STORE REAL CALLBACK TIMES
====================================================================================================

Code:
----------------------------------------------------------------------------------------------------
    if (index < MAX_SAMPLE)
        timestamps[index++] = ktime_get_ns();

Meaning:
----------------------------------------------------------------------------------------------------
record when the callback actually ran

After a few callbacks:
----------------------------------------------------------------------------------------------------
timestamps[0] = actual time of 1st callback
timestamps[1] = actual time of 2nd callback
timestamps[2] = actual time of 3rd callback
...

Why this is the correct measurement strategy:
----------------------------------------------------------------------------------------------------
It measures actual spacing between callback executions,
not just one initial elapsed time from module start.

COMMENT:
This is much better than repeatedly measuring:
    now - start_time

Because for periodic analysis, what you really care about is:

    callback_n - callback_(n-1)

That tells you interval stability.

====================================================================================================
DELTA ANALYSIS — WHAT THE PRINTED VALUES REALLY MEAN
====================================================================================================

On module exit:
----------------------------------------------------------------------------------------------------
    for (i = 1; i < index; i++)
        delta = timestamps[i] - timestamps[i - 1];

Meaning:
----------------------------------------------------------------------------------------------------
delta[i] = actual interval between two consecutive callbacks

Ideal values:
----------------------------------------------------------------------------------------------------
for 1 ms period:
    1,000,000 ns

for 1 second period:
    1,000,000,000 ns

Interpretation:
----------------------------------------------------------------------------------------------------
closer to target period
    = better timing precision and stability

COMMENT:
This is not just checking:
    "did callback run?"

It is checking:
    "how close was real spacing to the intended schedule?"

That is a much more senior way to judge timer behavior.

====================================================================================================
COMPLETE PERIODIC EVENT FLOW
====================================================================================================

module load
    |
    +--> initialize timer object
    +--> attach callback
    +--> define period
    +--> start timer
    |
    v
kernel keeps pending hrtimer ordered by expiration
    |
    v
expiration occurs
    |
    v
hrtimer_callback()
    |
    +--> record current timestamp
    +--> advance next expiration with hrtimer_forward_now()
    +--> return HRTIMER_RESTART
    |
    v
timer remains active
    |
    v
repeat until module unload or sample limit reached
    |
    v
module unload
    |
    +--> hrtimer_cancel()
    +--> print timing deltas
    +--> exit cleanly

COMMENT:
If you can replay this lifecycle in your head, you understand the example.

====================================================================================================
WHY hrtimer_cancel() IS REQUIRED
====================================================================================================

Code:
----------------------------------------------------------------------------------------------------
    hrtimer_cancel(&my_hrtimer);

Meaning:
----------------------------------------------------------------------------------------------------
stop the timer and ensure its callback will no longer run

Why mandatory:
----------------------------------------------------------------------------------------------------
the callback code lives in your module text
if the module unloads while timer is still active,
kernel might later try to execute code that no longer exists

That would be catastrophic.

COMMENT:
This is the same general kernel rule as:

    free_irq()
    del_timer_sync()
    cancel_work_sync()
    kthread_stop()

All of them mean:
    "shut down async execution before the owning code disappears"

Very important senior rule:
    if you started asynchronous execution,
    you must stop it before unload.

====================================================================================================
1 SECOND vs 1 MILLISECOND — WHY THIS DEMO MATTERS
====================================================================================================

Case A: 1 second
----------------------------------------------------------------------------------------------------
    period = ktime_set(1, 0)

Ideal delta:
    1,000,000,000 ns

This proves:
    hrtimer handles long precise intervals well

Case B: 1 millisecond
----------------------------------------------------------------------------------------------------
    period = ktime_set(0, 1000000)

Ideal delta:
    1,000,000 ns

This proves:
    hrtimer can schedule below one jiffy

If HZ = 250:
    one jiffy = 4 ms

So 1 ms is impossible to model accurately with plain jiffies-based timing.

COMMENT:
This is the strongest practical demonstration of why hrtimer exists.
It shows precision beyond the normal tick granularity.

====================================================================================================
COMPARISON TO LOW-RES GPIO DEBOUNCE TIMER
====================================================================================================

Low-resolution timer example:
----------------------------------------------------------------------------------------------------
    mod_timer(...)
    jiffies + msecs_to_jiffies(20)

Mental model:
    "wake me around 20 ms later"

Great for:
    debounce
    ordinary timeouts
    coarse delays

Hrtimer example:
----------------------------------------------------------------------------------------------------
    hrtimer_start(...)
    ktime_set(...)
    precise callback timing

Mental model:
    "wake me at this precise short interval"

COMMENT:
A very useful distinction:

    low-res timer
        = timeout tool

    hrtimer
        = precision scheduling tool

Both are valid.
They solve different problems.

====================================================================================================
INTERNAL KERNEL MENTAL MODEL
====================================================================================================

Low-res timer world:
----------------------------------------------------------------------------------------------------
tick-oriented
grouped/coalesced
optimized for efficiency

Hrtimer world:
----------------------------------------------------------------------------------------------------
exact-expiration-oriented
ordered by precise deadline
optimized for timing accuracy

COMMENT:
You do not need to memorize every internal implementation detail.
The useful senior mental model is:

    ordinary timer:
        "tick-bucket timeout system"

    hrtimer:
        "precise ordered deadline engine"

====================================================================================================
BEST SIMPLE MENTAL MODEL
====================================================================================================

hrtimer object
    = kernel-owned precise timer

ktime_t
    = scheduling time representation

callback
    = function kernel runs at expiration

one-shot mode
    = callback runs once, timer stops

periodic mode
    = callback runs, advances next deadline, restarts

timestamps[]
    = measurement log of what really happened

hrtimer_cancel()
    = safe shutdown of asynchronous timer activity

====================================================================================================
SENIOR COMMENTS TO HELP YOU REMEMBER
====================================================================================================

1. **An hrtimer is not a sleep**
It is a future callback registration.

2. **Init does not start**
`hrtimer_init()` prepares the object.
`hrtimer_start()` activates it.

3. **Periodic mode has two required parts**
You must:
    - move the next deadline
    - return RESTART

4. **Use CLOCK_MONOTONIC for elapsed-time logic**
Because interval timing should not depend on wall-clock changes.

5. **Measure real behavior, not assumed behavior**
That is why recording timestamps is so valuable.

6. **Cancel before unload**
Because the callback is asynchronous and still belongs to your module.

====================================================================================================
ONE-LINE FINAL TAKEAWAY
====================================================================================================

An hrtimer is a kernel precision scheduling object: you arm it against a stable clock, the kernel
calls your callback at expiration, and periodic behavior is built by forwarding the timer and
returning HRTIMER_RESTART.

====================================================================================================

```