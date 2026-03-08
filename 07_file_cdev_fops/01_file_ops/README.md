```log
========================================================================================================
CHAR DEVICE WITH FILE OPERATIONS — SENIOR MENTAL MODEL (ONE COMPLETE TEXT DIAGRAM)
========================================================================================================

USER SPACE                                                KERNEL SPACE
──────────                                                ────────────

echo "hello" > /dev/my_cdev0
cat /dev/my_cdev0
open("/dev/my_cdev0", O_RDWR)
read(fd, user_buf, count)
write(fd, user_buf, count)
close(fd)
      │
      │ 1) user uses a pathname under /dev
      ▼
┌──────────────────────────────────────────────────────────────────────────────────────────────────────┐
│ /dev/my_cdev0                                                                                        │
│                                                                                                      │
│ This is only a device node (a special file), not the driver itself.                                  │
│ It stores metadata such as:                                                                          │
│    - file type = character device                                                                    │
│    - major = assigned major                                                                          │
│    - minor = 0                                                                                       │
│                                                                                                      │
│ Why needed                                                                                           │
│    - gives user space a stable file-style entry point                                                │
│    - lets normal syscalls (open/read/write/close) reach the driver                                   │
│                                                                                                      │
│ Output                                                                                               │
│    - user-visible endpoint                                                                           │
└──────────────────────────────────────────────────────────────────────────────────────────────────────┘
      │
      │ 2) VFS resolves pathname, gets inode for this special file
      ▼
┌──────────────────────────────────────────────────────────────────────────────────────────────────────┐
│ VFS / INODE LAYER                                                                                    │
│                                                                                                      │
│ inode -> identifies WHICH device node was opened                                                     │
│ file  -> represents THIS SPECIFIC open instance                                                      │
│                                                                                                      │
│ inode contains device identity context:                                                              │
│    - imajor(inode) -> major                                                                          │
│    - iminor(inode) -> minor                                                                          │
│                                                                                                      │
│ file contains per-open runtime context:                                                              │
│    - f_pos    = current file offset for this open                                                    │
│    - f_mode   = access mode known by kernel                                                          │
│    - f_flags  = open flags such as O_RDONLY / O_WRONLY / O_RDWR / O_SYNC                             │
│    - private_data = driver-owned pointer for this open (not used yet, important later)               │
│                                                                                                      │
│ Why split inode and file                                                                             │
│    - inode answers: "which device is this?"                                                          │
│    - file answers:  "how is this current open instance behaving?"                                    │
│                                                                                                      │
│ Multiplicity                                                                                         │
│    - one device node/inode can be opened many times                                                  │
│    - each open gets its own struct file                                                              │
│                                                                                                      │
│ Parallelism                                                                                          │
│    - many processes can hold different struct file objects at the same time                          │
└──────────────────────────────────────────────────────────────────────────────────────────────────────┘
      │
      │ 3) kernel uses major/minor to find char-device registration
      ▼
┌──────────────────────────────────────────────────────────────────────────────────────────────────────┐
│ CHARACTER DEVICE REGISTRY                                                                            │
│                                                                                                      │
│ alloc_chrdev_region(&dev_nr, 0, MINORMASK+1, "my_cdev")                                              │
│                                                                                                      │
│ Kernel keeps a registry entry conceptually like:                                                     │
│                                                                                                      │
│    char device namespace                                                                             │
│       └── major = dynamically allocated                                                              │
│             ├── minor range = 0..255                                                                 │
│             └── name = "my_cdev"                                                                     │
│                                                                                                      │
│ Important detail                                                                                     │
│    - yes, kernel keeps the NAME together with the allocated range                                    │
│    - that is why /proc/devices can show something like:                                              │
│          236 my_cdev                                                                                 │
│                                                                                                      │
│ Why needed                                                                                           │
│    - reserves the device-number namespace                                                            │
│    - prevents collisions with other drivers                                                          │
│    - gives kernel a registered identity for this driver family                                       │
│                                                                                                      │
│ Multiplicity                                                                                         │
│    - one major can cover many minors                                                                 │
│    - many logical instances may live under one major                                                 │
│                                                                                                      │
│ Output                                                                                               │
│    - device-number ownership exists inside kernel                                                    │
│    - name becomes visible in /proc/devices                                                           │
└──────────────────────────────────────────────────────────────────────────────────────────────────────┘
      │
      │ 4) kernel dispatches major/minor into cdev
      ▼
┌──────────────────────────────────────────────────────────────────────────────────────────────────────┐
│ CDEV DISPATCH LAYER                                                                                  │
│                                                                                                      │
│ cdev_init(&my_cdev, &fops)                                                                           │
│ my_cdev.owner = THIS_MODULE                                                                          │
│ cdev_add(&my_cdev, dev_nr, MINORMASK+1)                                                              │
│                                                                                                      │
│ Conceptual mapping inside kernel:                                                                    │
│                                                                                                      │
│    (major, minor)                                                                                    │
│         │                                                                                            │
│         ▼                                                                                            │
│      struct cdev                                                                                     │
│         │                                                                                            │
│         ▼                                                                                            │
│      struct file_operations                                                                          │
│         ├── .open    = my_open                                                                       │
│         ├── .release = my_release                                                                    │
│         ├── .read    = my_read                                                                       │
│         └── .write   = my_write                                                                      │
│                                                                                                      │
│ Why needed                                                                                           │
│    - device numbers alone are only identity                                                          │
│    - cdev is what binds that identity to executable driver behavior                                  │
│                                                                                                      │
│ owner = THIS_MODULE                                                                                  │
│    - prevents unloading while file operations are in use                                             │
│                                                                                                      │
│ Multiplicity                                                                                         │
│    - one cdev may represent a whole minor range                                                      │
│    - or a driver may use multiple cdev objects                                                       │
│                                                                                                      │
│ Output                                                                                               │
│    - open/read/write on the device can now reach driver callbacks                                    │
└──────────────────────────────────────────────────────────────────────────────────────────────────────┘
      │
      │ 5) sysfs/device-model publication
      ▼
┌──────────────────────────────────────────────────────────────────────────────────────────────────────┐
│ DEVICE MODEL LAYER                                                                                   │
│                                                                                                      │
│ class_create("my_class")                                                                             │
│ device_create(my_class, NULL, dev_nr, NULL, "my_cdev%d", 0)                                          │
│                                                                                                      │
│ Creates sysfs objects conceptually like:                                                             │
│                                                                                                      │
│    /sys/class/my_class                                                                               │
│    /sys/class/my_class/my_cdev0                                                                      │
│                                                                                                      │
│ Why needed                                                                                           │
│    - publishes the device into the Linux device model                                                │
│    - allows udev to observe an add event                                                             │
│    - separates driver dispatch from system discoverability                                           │
│                                                                                                      │
│ Important distinction                                                                                │
│    - alloc_chrdev_region name = char-device registry name                                            │
│    - device_create name     = concrete device instance name                                          │
│    - these are related, but not the same layer                                                       │
│                                                                                                      │
│ Multiplicity                                                                                         │
│    - one class can contain many device instances                                                     │
│    - device_create can be called repeatedly for my_cdev0, my_cdev1, ...                              │
│                                                                                                      │
│ Output                                                                                               │
│    - sysfs class + sysfs device instance                                                             │
│    - uevent sent to user space                                                                       │
└──────────────────────────────────────────────────────────────────────────────────────────────────────┘
      │
      │ 6) udev reacts to kernel event
      ▼
┌──────────────────────────────────────────────────────────────────────────────────────────────────────┐
│ UDEV / USERSPACE DEVICE MANAGEMENT                                                                   │
│                                                                                                      │
│ udev receives add event for the new device                                                           │
│ and creates /dev/my_cdev0                                                                            │
│                                                                                                      │
│ Why needed                                                                                           │
│    - automates creation/removal of device nodes                                                      │
│    - avoids manual mknod                                                                             │
│                                                                                                      │
│ Output                                                                                               │
│    - /dev/my_cdev0 appears automatically                                                             │
└──────────────────────────────────────────────────────────────────────────────────────────────────────┘
      │
      │ 7) open() path
      ▼
┌──────────────────────────────────────────────────────────────────────────────────────────────────────┐
│ my_open(inode, file)                                                                                 │
│                                                                                                      │
│ Current lesson behavior                                                                              │
│    - prints major/minor                                                                              │
│    - prints f_pos, f_mode, f_flags                                                                   │
│    - returns 0                                                                                       │
│                                                                                                      │
│ Senior interpretation                                                                                │
│    - open is where a real driver often identifies the target instance                                │
│    - may validate access mode                                                                        │
│    - may set file->private_data                                                                      │
│    - may increment open count                                                                        │
│                                                                                                      │
│ Current output                                                                                       │
│    - diagnostics only                                                                                │
│                                                                                                      │
│ Multiplicity                                                                                         │
│    - runs once per successful open                                                                   │
│    - many opens may happen in parallel                                                               │
└──────────────────────────────────────────────────────────────────────────────────────────────────────┘
      │
      │ 8) write() path: user -> kernel
      ▼
┌──────────────────────────────────────────────────────────────────────────────────────────────────────┐
│ SHARED DEVICE STATE                                                                                  │
│                                                                                                      │
│ static char *dev_buffer                                                                              │
│ #define DEV_BUFFER_SIZE 64                                                                           │
│ static struct mutex dev_mutex                                                                        │
│                                                                                                      │
│ my_init() allocates this buffer with kmalloc(..., GFP_KERNEL)                                        │
│ and clears it with memset(..., 0, DEV_BUFFER_SIZE)                                                   │
│                                                                                                      │
│ Why needed                                                                                           │
│    - this lesson has no real hardware                                                                │
│    - buffer acts as the device's internal storage                                                    │
│                                                                                                      │
│ Why mutex needed                                                                                     │
│    - dev_buffer is shared mutable state                                                              │
│    - multiple processes may read/write at the same time                                              │
│                                                                                                      │
│ Output                                                                                               │
│    - kernel-owned storage backing the device                                                         │
└──────────────────────────────────────────────────────────────────────────────────────────────────────┘
      │
      │ write(fd, user_buf, count)
      ▼
┌──────────────────────────────────────────────────────────────────────────────────────────────────────┐
│ my_write(file, user_buf, count, pOffset)                                                             │
│                                                                                                      │
│ Sequence                                                                                             │
│   1. mutex_lock_interruptible(&dev_mutex)                                                            │
│      - serialize access to shared buffer                                                             │
│      - if interrupted, return -ERESTARTSYS                                                           │
│                                                                                                      │
│   2. check remaining capacity using *pOffset and DEV_BUFFER_SIZE                                     │
│      - if offset >= size, return -ENOSPC                                                             │
│      - else limit bytes_to_copy to remaining capacity                                                │
│                                                                                                      │
│   3. copy_from_user(dev_buffer + *pOffset, user_buf, bytes_to_copy)                                  │
│      - crosses user/kernel boundary safely                                                           │
│      - returns NOT COPIED bytes, not copied bytes                                                    │
│                                                                                                      │
│   4. copied = bytes_to_copy - not_copied                                                             │
│                                                                                                      │
│   5. *pOffset += copied                                                                              │
│      - advances file position for THIS open instance                                                 │
│                                                                                                      │
│   6. mutex_unlock(&dev_mutex)                                                                        │
│                                                                                                      │
│   7. return copied                                                                                   │
│                                                                                                      │
│ Why needed                                                                                           │
│    - accepts data from user space and stores it in kernel-owned state                                │
│                                                                                                      │
│ Output                                                                                               │
│    - device buffer content changes                                                                   │
│    - file position advances                                                                          │
│    - return value tells user how many bytes were accepted                                            │
│                                                                                                      │
│ Parallelism                                                                                          │
│    - many writers may arrive concurrently                                                            │
│    - mutex makes write path mutually exclusive                                                       │
└──────────────────────────────────────────────────────────────────────────────────────────────────────┘
      │
      │ 9) read() path: kernel -> user
      ▼
┌──────────────────────────────────────────────────────────────────────────────────────────────────────┐
│ my_read(file, user_buf, count, pOffset)                                                              │
│                                                                                                      │
│ Sequence                                                                                             │
│   1. mutex_lock_interruptible(&dev_mutex)                                                            │
│                                                                                                      │
│   2. determine how many bytes can be returned                                                        │
│      current lesson uses string-style idea:                                                          │
│         bytes_to_copy based on strlen(dev_buffer) and *pOffset                                       │
│                                                                                                      │
│      learning intent:                                                                                │
│         stop reading when logical end of stored content is reached                                   │
│                                                                                                      │
│   3. copy_to_user(user_buf, dev_buffer + *pOffset, bytes_to_copy)                                    │
│      - safe kernel -> user transfer                                                                  │
│      - returns NOT COPIED bytes                                                                      │
│                                                                                                      │
│   4. copied = bytes_to_copy - not_copied                                                             │
│                                                                                                      │
│   5. *pOffset += copied                                                                              │
│                                                                                                      │
│   6. mutex_unlock(&dev_mutex)                                                                        │
│                                                                                                      │
│   7. return copied                                                                                   │
│                                                                                                      │
│ Read return semantics                                                                                │
│    - >0 : bytes returned                                                                             │
│    -  0 : EOF / no more data                                                                         │
│    - <0 : error                                                                                      │
│                                                                                                      │
│ Why cat reads twice                                                                                  │
│    - first read gets actual data                                                                     │
│    - second read gets 0                                                                              │
│    - that 0 tells cat "end of file"                                                                  │
│                                                                                                      │
│ Output                                                                                               │
│    - user buffer receives bytes                                                                      │
│    - file position advances                                                                          │
└──────────────────────────────────────────────────────────────────────────────────────────────────────┘
      │
      │ 10) close() path
      ▼
┌──────────────────────────────────────────────────────────────────────────────────────────────────────┐
│ my_release(inode, file)                                                                              │
│                                                                                                      │
│ Current lesson behavior                                                                              │
│    - prints that file is closed                                                                      │
│    - returns 0                                                                                       │
│                                                                                                      │
│ Senior interpretation                                                                                │
│    - release is where a real driver may free per-open resources                                      │
│    - may decrement open count                                                                        │
│    - may flush final state                                                                           │
│                                                                                                      │
│ Multiplicity                                                                                         │
│    - runs once for each final close of a struct file instance                                        │
└──────────────────────────────────────────────────────────────────────────────────────────────────────┘
      │
      ▼
========================================================================================================
STATE MODEL OF THIS LESSON
========================================================================================================

GLOBAL / SHARED DEVICE STATE
    dev_buffer
    dev_mutex
    cdev registration
    class/device publication

PER-OPEN STATE
    struct file
        - f_pos
        - f_mode
        - f_flags
        - private_data (not used yet)

This means:
    - all opens share the same underlying device buffer
    - each open has its own current file offset
    - concurrent access is possible
    - mutex protects shared buffer, not each file separately

========================================================================================================
KEY SENIOR TAKEAWAY
========================================================================================================

This driver is no longer just "a registered char device".
It is now a real file-like kernel service with:

    identity          -> dev_t / major / minor / name
    dispatch          -> cdev + file_operations
    publication       -> class + device + udev
    shared state      -> dev_buffer
    synchronization   -> mutex
    per-open context  -> struct file (f_pos/f_mode/f_flags)
    user/kernel copy  -> copy_to_user / copy_from_user

So the complete mental model is:

    /dev node
        -> VFS inode/file
        -> major/minor lookup
        -> cdev
        -> file_operations
        -> shared kernel buffer with locking
        -> bytes move safely across the user/kernel boundary

========================================================================================================
```