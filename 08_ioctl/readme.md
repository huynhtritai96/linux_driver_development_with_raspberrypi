## Device Drivers IOCTL

### Video :
[![Youtube Video](https://img.youtube.com/vi/EvgRY-rU4jQ/0.jpg)](https://www.youtube.com/watch?v=EvgRY-rU4jQ)

```
========================================================================================================
PRIVATE DATA IN A CHARACTER DEVICE — SENIOR MENTAL MODEL (ONE COMPLETE TEXT DIAGRAM)
========================================================================================================
GOAL OF THIS VERSION
────────────────────────────────────────────────────────────────────────────────────────────────────────
Previous version:
    one GLOBAL kernel buffer shared by all opens

This version:
    one PRIVATE kernel buffer per open file instance
    stored in:
        struct file -> private_data

So the design changes from:
    shared device state
        ↓
    all processes use same buffer

to:
    per-open state
        ↓
    each open gets its own buffer

========================================================================================================
LIFETIME MODEL
========================================================================================================

MODULE LOAD TIME                                  OPEN TIME                                  CLOSE TIME
───────────────                                   ─────────                                  ──────────
insmod my_cdev.ko                                 open("/dev/my_cdev0")                      close(fd)
      │                                                   │                                      │
      ▼                                                   ▼                                      ▼
┌───────────────────────┐                    ┌──────────────────────────────┐        ┌──────────────────────────────┐
│ module init runs      │                    │ my_open(inode, file)         │        │ my_release(inode, file)      │
│                       │                    │                              │        │                              │
│ creates cdev/class/   │                    │ kzalloc(64)                  │        │ buffer = file->private_data  │
│ /dev node             │                    │     ↓                        │        │ if (buffer) kfree(buffer)    │
│                       │                    │ returns per-open buffer      │        │                              │
│ BUT NO DATA BUFFER    │                    │                              │        │ per-open memory disappears   │
│ is allocated here     │                    │ file->private_data = buffer  │        │ when this open instance ends │
└───────────────────────┘                    └──────────────────────────────┘        └──────────────────────────────┘

Key idea:
    device registration lifetime != data-buffer lifetime

The char device exists for the module lifetime.
The private buffer exists only for one open file lifetime.

========================================================================================================
SYSTEM STACK
========================================================================================================

USER PROCESS
    │
    │ open / write / read / close
    ▼
/dev/my_cdev0
    │
    │ device node with major/minor metadata
    ▼
VFS
    │
    ├── inode  = which device node is this?
    └── file   = this specific open instance
                     │
                     ├── f_pos
                     ├── f_mode
                     ├── f_flags
                     └── private_data   <========================== IMPORTANT FIELD
    ▼
cdev lookup by major/minor
    ▼
file_operations
    ├── .open    = my_open
    ├── .read    = my_read
    ├── .write   = my_write
    └── .release = my_release

Why private_data exists:
    - VFS gives the driver one struct file per successful open
    - driver needs a place to attach per-open context
    - private_data is that attachment point

private_data type:
    void *
meaning:
    driver may store any pointer there:
        - char buffer
        - device context struct
        - session object
        - transaction state
        - DMA context
        - per-open flags

In this tutorial:
    private_data points to a 64-byte char buffer

========================================================================================================
BOOT / REGISTRATION LAYER (same as previous lesson)
========================================================================================================

my_init()
    │
    ├── mutex_init(&dev_mutex)
    ├── alloc_chrdev_region(&dev_nr, 0, MINORMASK+1, "my_cdev")
    ├── cdev_init(&my_cdev, &fops)
    ├── cdev_add(&my_cdev, dev_nr, MINORMASK+1)
    ├── class_create("my_class")
    └── device_create(my_class, NULL, dev_nr, NULL, "my_cdev%d", 0)

Outputs:
    /proc/devices             -> major + name
    /sys/class/my_class       -> class
    /sys/class/my_class/my_cdev0
    /dev/my_cdev0             -> user entry point

Important difference from previous version:
    init() no longer allocates a global data buffer

Why that matters:
    - memory is not consumed just because module is loaded
    - data storage now appears only when a user actually opens the device
    - this is more scalable for multi-open designs

========================================================================================================
OPEN PATH — WHERE PRIVATE DATA IS CREATED
========================================================================================================

User does:
    fd = open("/dev/my_cdev0", O_RDWR);

Kernel path:
    open syscall
        ↓
    VFS resolves /dev/my_cdev0
        ↓
    inode + file prepared
        ↓
    cdev/file_operations lookup
        ↓
    my_open(inode, file)

Inside my_open():
    char *buffer = kzalloc(DEV_BUFFER_SIZE, GFP_KERNEL);

What kzalloc gives:
    - dynamically allocated kernel memory
    - already zero-filled
    - safe starting state for string-like demo behavior

If allocation fails:
    return -ENOMEM

If allocation succeeds:
    file->private_data = buffer

Meaning:
    this specific open file instance now owns this specific kernel buffer

Diagram of a single open:
    struct file (for fd #3)
        ├── f_pos
        ├── f_mode
        ├── f_flags
        └── private_data ───────────────► [ 64-byte kernel buffer, zeroed ]

Why open() is the right place:
    - this is the first point where the driver knows a new open instance exists
    - per-open resources should normally be acquired here
    - private_data becomes the bridge from generic VFS file to driver-specific state

Output of open():
    one new open-instance object in kernel with one attached private buffer

========================================================================================================
WRITE PATH — USER DATA ENTERS THE PRIVATE BUFFER
========================================================================================================

User does:
    write(fd, user_buf, count);

Kernel path:
       user buffer
        ↓
      write syscall
        ↓
       VFS
        ↓
       my_write(file, user_buf, count, pOffset)

Inside my_write():
    1. dev_buffer = (char *)file->private_data
    2. if !dev_buffer -> return -EINVAL
    3. lock mutex
    4. bytes_to_copy = min(count, DEV_BUFFER_SIZE)
    5. copy_from_user(dev_buffer, user_buf, bytes_to_copy)
    6. unlock mutex
    7. return copied

Data movement:
    USER SPACE BUFFER  ----------------------copy_from_user---------------------->  KERNEL PRIVATE BUFFER

Meaning:
    - write no longer stores data in a global shared module buffer
    - it stores data in the buffer belonging to THIS open instance only

Output of write():
    private buffer content now holds what this file instance wrote

Important semantic change:
    with private_data, "device state" is now closer to:
        session state per open
    not:
        one global device memory for everyone

========================================================================================================
READ PATH — PRIVATE BUFFER RETURNS DATA TO USER
========================================================================================================

User does:
    read(fd, user_buf, count);

Kernel path:
    my_read(file, user_buf, count, pOffset)

Inside my_read():
    1. dev_buffer = (char *)file->private_data
    2. if !dev_buffer -> return -EINVAL
    3. lock mutex
    4. bytes_to_copy = min(count, strlen(dev_buffer))
    5. copy_to_user(user_buf, dev_buffer, bytes_to_copy)
    6. unlock mutex
    7. return copied

Data movement:
    KERNEL PRIVATE BUFFER  ------------------------copy_to_user------------------>  USER SPACE BUFFER

Meaning:
    read returns the contents attached to THIS open instance

Output of read():
    user gets back the string/data stored in that file's private buffer

Why this works independently across two terminals:
    terminal A opens device -> gets buffer A
    terminal B opens device -> gets buffer B

So:
    terminal A write("hello")
    terminal B write("world")

produces:
    file A private_data -> "hello"
    file B private_data -> "world"

No collision between the two open instances at the data-storage level.

========================================================================================================
CLOSE PATH — WHERE PRIVATE DATA IS DESTROYED
========================================================================================================

User does:
    close(fd);

Kernel path:
    my_release(inode, file)

Inside my_release():
    char *buffer = (char *)file->private_data;
    if (buffer)
        kfree(buffer);

Meaning:
    the per-open resource is released when that open instance ends

Diagram:
    before close:
        struct file
            └── private_data ─────────────► allocated buffer

    after close:
        struct file destroyed by VFS
        buffer freed by driver
        no per-open data remains

Why release() is the right place:
    - it mirrors open()
    - resource acquisition and release stay paired
    - avoids memory leak for each open instance

========================================================================================================
GLOBAL BUFFER MODEL vs PRIVATE_DATA MODEL
========================================================================================================

OLD MODEL: GLOBAL BUFFER
────────────────────────────────────────────────────────────────────────────────────────────────────────
module load
    alloc global dev_buffer once
open A
open B
write from A modifies same buffer
read from B sees same shared data

shape:
    module
      └── global dev_buffer  <---- shared by all opens

properties:
    - memory allocated even when nobody uses device
    - all opens share same content
    - easier to accidentally mix states
    - good for learning "shared device memory"
    - behaves like one device-wide storage area

NEW MODEL: PRIVATE DATA BUFFER
────────────────────────────────────────────────────────────────────────────────────────────────────────
module load
    no data buffer yet
open A -> alloc buffer A
open B -> alloc buffer B
write from A modifies buffer A only
read from B sees buffer B only
close A -> free buffer A
close B -> free buffer B

shape:
    open file A
      └── private_data -> buffer A

    open file B
      └── private_data -> buffer B

properties:
    - memory exists only while file is open
    - each open instance can keep independent state
    - better for session-like behavior
    - scales more naturally to multi-process access
    - much closer to how real drivers store per-open context

========================================================================================================
MULTI-INSTANCE VIEW (THE MOST IMPORTANT IDEA OF THIS LESSON)
========================================================================================================

Two terminals open the same device node at the same time:
Terminal 1                              Terminal 2
──────────                              ──────────
open("/dev/my_cdev0")                   open("/dev/my_cdev0")
      │                                       │
      ▼                                       ▼
struct file F1                           struct file F2
      └── private_data ─────► buffer1          └── private_data ─────► buffer2

write(F1, "Welcome")
read(F1)  -> "Welcome"

write(F2, "MP Coding")
read(F2)  -> "MP Coding"

Why this is possible:
    - /dev/my_cdev0 is one device node
    - but every open gets its own struct file
    - private_data belongs to struct file
    - therefore private_data is naturally per-open, not per-device

This is the core learning outcome:
    one device file can have many simultaneous open instances
    and each instance can maintain independent driver state

========================================================================================================
WHAT THE MUTEX IS PROTECTING IN THIS VERSION
========================================================================================================

Current code still uses one global mutex:
    static struct mutex dev_mutex;

So read/write are serialized across all opens.

What it protects:
    - copy/read/write sections
    - access to current private buffers during operation

But note carefully:
    mutex is GLOBAL
    buffer is PER-OPEN

That means:
    terminal A and terminal B do not share the same buffer, but they still cannot read/write in parallel because one global mutex blocks both paths.

Better future design for per-open state:
    store a struct in private_data:
        struct my_file_ctx {
            char buffer[64];
            struct mutex lock;
            size_t len;
        };

Then:
    file->private_data -> my_file_ctx
and each open instance gets its own lock + buffer + length

That is the senior design direction.

========================================================================================================
WHY OFFSET WAS REMOVED IN THIS TUTORIAL VERSION
========================================================================================================

Previous lesson used:
    *pOffset as a file position into one shared buffer

That made the device behave more like a sequential file stream.

In this private_data lesson, the tutorial simplifies behavior:
    - ignore offset
    - always write from start of private buffer
    - always read current content from start of private buffer

Why done for teaching:
    because the main point of the lesson is:
        private_data creates per-open state
    not:
        file-position semantics

Tradeoff:
    easier to understand private ownership,
    but less file-like and less realistic than a true offset-aware driver

========================================================================================================
OUTPUTS AT EACH STAGE
========================================================================================================

Module load output:
    /dev/my_cdev0 exists
    device registered

Open output:
    one private buffer allocated
    file->private_data now points to it

Write output: data copied from user space into this file instance's private buffer
Read output: data copied from this file instance's private buffer back to user space

Close output: private buffer freed

System-visible output:
    same /dev/my_cdev0 node

Behavior-visible output:
    different opens can hold different independent data

========================================================================================================
KEY SENIOR TAKEAWAY
========================================================================================================

private_data is not "extra storage inside the driver";
it is the standard VFS hook that lets a driver attach per-open context to struct file.

So the real model is:
    one device node
        ↓
    many opens
        ↓
    many struct file objects
        ↓
    each struct file may carry its own private_data
        ↓
    each open instance can behave independently

That is why private_data is one of the most important fields in Linux file-based driver design.

========================================================================================================
```