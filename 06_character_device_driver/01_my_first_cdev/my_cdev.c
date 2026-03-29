#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("MPCoding - LDD");
MODULE_DESCRIPTION("Our first character device");

static const char *my_device = "my_cdev"; // device name
static dev_t dev_nr;                    // device number (major and minor number)
static struct cdev my_cdev;             // character device structure
static struct class *my_class;          // class structure for creating device number under /sys/class and device node under /dev
static struct device *my_device_object; // device object structure for creating device node under /dev
static struct file_operations fops = {  //  file characteristic for device file/node file
};

static int __init my_init(void)
{
    int status;
    status = alloc_chrdev_region(&dev_nr, 0, MINORMASK + 1, my_device);
    if (status)
    {
        pr_err("%s: character device registation failed\n", my_device);
        return status;
    }

    // cat /proc/devices --  → 236 my_cdev -- the kernel stores the device name along with the allocated dev_nr, so that it can be displayed in /proc/devices and used by udev to create the device node under /dev with the correct name and major/minor numbers.
    cdev_init(&my_cdev, &fops);
    my_cdev.owner = THIS_MODULE;

    status = cdev_add(&my_cdev, dev_nr, MINORMASK + 1);
    if (status)
    {
        pr_err("%s: error adding cdev\n", my_device);
        goto free_device_nr; // if cdev_add fails, we need to free the device number that we allocated with alloc_chrdev_region, so we jump to the free_device_nr label to do that.
    }

    // create: /sys/class/my_class  
    my_class = class_create("my_class");
    if (IS_ERR(my_class)) // class_create returns a pointer, so we need to check if it is an error pointer using IS_ERR macro.
    {
        pr_err("%s: Could not create class my_class\n",my_device);
        status = ENOMEM;
        goto delete_cdev; // if class_create fails, we need to delete the cdev that we added with cdev_add + free the device number that we allocated with alloc_chrdev_region, so we jump to the delete_cdev label to do that.
    }

    // create: /sys/class/my_class/my_cdev0 and /dev/my_cdev0
    my_device_object = device_create(my_class, NULL, dev_nr, NULL, "my_cdev%d", 0);
    if (IS_ERR(my_device_object)) // device_create returns a pointer, so we need to check if it is an error pointer using IS_ERR macro.
    { 
        pr_err("%s: Could not create device my_cdev0\n", my_device);
        status = ENOMEM;
        goto delete_class; // if device_create fails, we need to destroy the class that we created with class_create + delete the cdev that we added with cdev_add + free the device number that we allocated with alloc_chrdev_region, so we jump to the delete_class label to do that.
    }

    pr_info("%s: Character device registerd, Major number: %d Minor number: %d\n",my_device, MAJOR(dev_nr), MINOR(dev_nr));
    pr_info("%s: Created device entry under /sys/class/my_class\n", my_device); // /sys/class/my_class contains a device object, not a “device number”.
    pr_info("%s: Created new device node /dev/my_cdev\n", my_device);
    return 0;

delete_class: // remove the duplicate code for destroying the class and deleting the cdev, we can just jump to the delete_cdev label to do that, since it will also free the device number that we allocated with alloc_chrdev_region.
    class_destroy(my_class);

delete_cdev:
    cdev_del(&my_cdev);

free_device_nr:
    unregister_chrdev_region(dev_nr, MINORMASK + 1); 

    return status;
}

static void __exit my_exit(void)
{
    device_destroy(my_class, dev_nr);

    class_destroy(my_class);
    cdev_del(&my_cdev);
    unregister_chrdev_region(dev_nr, MINORMASK + 1);
    pr_info("%s: Goodbye, Kernel\n", my_device);
}

module_init(my_init);
module_exit(my_exit);

/*
    major = register_chrdev(0, "my_cdev", &fops); // similar to alloc_chrdev_region but it only returns the major number and it does not support dynamic allocation of minor numbers, so we have to specify the minor number in the file operations structure. It is also deprecated and should not be used in new code.
    if (major < 0) {
        pr_err("%s: character device registation failed\n", my_device);
        return major;
    }
*/

/*
Below is a **single detailed text diagram** that a **senior kernel developer would mentally visualize** when reading or designing this code. It shows **objects, ownership, identity, data flow, and system outputs**, including the detail you noticed: **the kernel stores the device name along with the allocated `dev_nr`**.

---

```
====================================================================================================
LINUX CHARACTER DEVICE CREATION FLOW (MENTAL MODEL OF THE SYSTEM)
====================================================================================================

MODULE LOAD
insmod my_cdev.ko
        │
        ▼
┌──────────────────────────────────────────────────────────────────────────────────────────────────┐
│ 1. DRIVER DEFINES STATIC OBJECTS (Module-owned lifetime)                                         │
│                                                                                                  │
│   static const char *my_device = "my_cdev";                                                       │
│   static dev_t dev_nr;                                                                           │
│   static struct cdev my_cdev;                                                                    │
│   static struct class *my_class;                                                                 │
│   static struct file_operations fops;                                                            │
│                                                                                                  │
│   Purpose                                                                                        │
│   -------                                                                                        │
│   These are long-lived kernel objects owned by the module.                                       │
│                                                                                                  │
│   Kernel does NOT know about them yet.                                                            │
│                                                                                                  │
│   Multiplicity                                                                                   │
│   -----------                                                                                    │
│   file_operations  → usually 1 per driver                                                         │
│   cdev             → 1 or many per driver                                                         │
│   class            → usually 1 per device family                                                  │
│                                                                                                  │
│   Output                                                                                         │
│   ------                                                                                         │
│   Nothing visible to system yet (pure internal definitions).                                      │
└──────────────────────────────────────────────────────────────────────────────────────────────────┘
        │
        ▼
┌──────────────────────────────────────────────────────────────────────────────────────────────────┐
│ 2. DEVICE NUMBER ALLOCATION                                                                      │
│                                                                                                  │
│   alloc_chrdev_region(&dev_nr, 0, MINORMASK+1, "my_cdev")                                         │
│                                                                                                  │
│   Kernel internally creates an entry in the character device registry:                          │
│                                                                                                  │
│        char_device_registry                                                                      │
│             │                                                                                    │
│             ├── major = dynamically chosen (example: 236)                                        │
│             │       │                                                                             │
│             │       ├── minor range: 0..255                                                       │
│             │       │                                                                             │
│             │       └── name = "my_cdev"                                                          │
│             │             (kernel stores this name for /proc/devices display)                    │
│                                                                                                  │
│   Important observation                                                                          │
│   ---------------------                                                                          │
│   The kernel stores BOTH:                                                                        │
│        major/minor range  +  device name                                                         │
│                                                                                                  │
│   This is why you later see:                                                                     │
│        cat /proc/devices                                                                         │
│        → 236 my_cdev                                                                             │
│                                                                                                  │
│   Multiplicity                                                                                   │
│   -----------                                                                                    │
│   One major number may represent MANY device instances.                                          │
│                                                                                                  │
│   Example                                                                                        │
│                                                                                                  │
│        major 236                                                                                 │
│            ├─ minor 0 → my_cdev0                                                                  │
│            ├─ minor 1 → my_cdev1                                                                  │
│            ├─ minor 2 → my_cdev2                                                                  │
│            └─ minor 3 → my_cdev3                                                                  │
│                                                                                                  │
│   Output                                                                                         │
│   ------                                                                                         │
│   Kernel now reserves this namespace for the driver.                                             │
└──────────────────────────────────────────────────────────────────────────────────────────────────┘
        │
        ▼
┌──────────────────────────────────────────────────────────────────────────────────────────────────┐
│ 3. DRIVER BINDS OPERATIONS TO DEVICE NUMBERS                                                     │
│                                                                                                  │
│   cdev_init(&my_cdev, &fops)                                                                     │
│   my_cdev.owner = THIS_MODULE                                                                    │
│                                                                                                  │
│   cdev_add(&my_cdev, dev_nr, MINORMASK+1)                                                         │
│                                                                                                  │
│   Kernel updates internal dispatch structure:                                                    │
│                                                                                                  │
│        char_device_map                                                                           │
│             │                                                                                    │
│             └── major 236                                                                        │
│                   │                                                                              │
│                   └── minor 0..255                                                               │
│                         │                                                                        │
│                         └── struct cdev                                                          │
│                                 │                                                                │
│                                 └── file_operations                                              │
│                                         │                                                        │
│                                         ├── open()                                               │
│                                         ├── read()                                               │
│                                         ├── write()                                              │
│                                         └── ioctl()                                              │
│                                                                                                  │
│   Now the kernel routing logic exists.                                                           │
│                                                                                                  │
│   Runtime path will later be:                                                                    │
│                                                                                                  │
│        user open("/dev/my_cdev0")                                                                │
│              │                                                                                   │
│              ▼                                                                                   │
│        VFS extracts major/minor                                                                  │
│              │                                                                                   │
│              ▼                                                                                   │
│        char_device_map lookup                                                                    │
│              │                                                                                   │
│              ▼                                                                                   │
│        cdev → file_operations → driver                                                           │
│                                                                                                  │
│   Output                                                                                         │
│   ------                                                                                         │
│   Driver is now active in kernel char-device subsystem.                                          │
│                                                                                                  │
│   Visible in:                                                                                    │
│        /proc/devices                                                                             │
└──────────────────────────────────────────────────────────────────────────────────────────────────┘
        │
        ▼
┌──────────────────────────────────────────────────────────────────────────────────────────────────┐
│ 4. DEVICE MODEL INTEGRATION (sysfs layer)                                                        │
│                                                                                                  │
│   my_class = class_create("my_class")                                                            │
│                                                                                                  │
│   Kernel creates:                                                                                │
│        /sys/class/my_class                                                                       │
│                                                                                                  │
│   This represents a logical device family.                                                       │
│                                                                                                  │
│   Examples in real systems                                                                       │
│                                                                                                  │
│        /sys/class/net                                                                            │
│        /sys/class/tty                                                                            │
│        /sys/class/input                                                                          │
│                                                                                                  │
│   Multiplicity                                                                                   │
│   -----------                                                                                    │
│   One class can contain many device instances.                                                   │
│                                                                                                  │
│   Output                                                                                         │
│   ------                                                                                         │
│   Device family visible in sysfs.                                                                │
└──────────────────────────────────────────────────────────────────────────────────────────────────┘
        │
        ▼
┌──────────────────────────────────────────────────────────────────────────────────────────────────┐
│ 5. DEVICE INSTANCE CREATION                                                                      │
│                                                                                                  │
│   device_create(my_class, NULL, dev_nr, NULL, "my_cdev%d", 0)                                    │
│                                                                                                  │
│   Kernel creates sysfs device object:                                                            │
│                                                                                                  │
│        /sys/class/my_class/my_cdev0                                                              │
│                                                                                                  │
│   The kernel then emits a hotplug event:                                                         │
│                                                                                                  │
│        UEVENT: "add device major=236 minor=0 name=my_cdev0"                                      │
│                                                                                                  │
│   Output                                                                                         │
│   ------                                                                                         │
│   Device instance visible in sysfs.                                                              │
└──────────────────────────────────────────────────────────────────────────────────────────────────┘
        │
        ▼
┌──────────────────────────────────────────────────────────────────────────────────────────────────┐
│ 6. USERSPACE DEVICE NODE CREATION (udev)                                                         │
│                                                                                                  │
│   udev receives kernel event.                                                                    │
│                                                                                                  │
│   Using device metadata (major/minor + name), it creates:                                       │
│                                                                                                  │
│        /dev/my_cdev0                                                                             │
│                                                                                                  │
│   This node contains:                                                                            │
│                                                                                                  │
│        type  = character device                                                                  │
│        major = 236                                                                               │
│        minor = 0                                                                                 │
│                                                                                                  │
│   Output                                                                                         │
│   ------                                                                                         │
│   User-visible device endpoint.                                                                  │
└──────────────────────────────────────────────────────────────────────────────────────────────────┘
        │
        ▼
┌──────────────────────────────────────────────────────────────────────────────────────────────────┐
│ 7. RUNTIME DATA FLOW                                                                             │
│                                                                                                  │
│   User program:                                                                                  │
│        open("/dev/my_cdev0")                                                                     │
│        read()                                                                                    │
│        write()                                                                                   │
│                                                                                                  │
│   Kernel path:                                                                                   │
│                                                                                                  │
│        User process                                                                              │
│             │                                                                                    │
│             ▼                                                                                    │
│        System call                                                                               │
│             │                                                                                    │
│             ▼                                                                                    │
│        VFS                                                                                       │
│             │                                                                                    │
│             ▼                                                                                    │
│        Extract major/minor from inode                                                            │
│             │                                                                                    │
│             ▼                                                                                    │
│        char_device_map lookup                                                                    │
│             │                                                                                    │
│             ▼                                                                                    │
│        cdev → file_operations                                                                    │
│             │                                                                                    │
│             ▼                                                                                    │
│        Driver code executes                                                                      │
│                                                                                                  │
│   Parallelism                                                                                    │
│   -----------                                                                                    │
│   Multiple processes may simultaneously access:                                                  │
│        /dev/my_cdev0                                                                             │
│        /dev/my_cdev1                                                                             │
│        etc.                                                                                      │
│                                                                                                  │
│   Therefore drivers must handle concurrency and synchronization.                                │
└──────────────────────────────────────────────────────────────────────────────────────────────────┘
        │
        ▼
====================================================================================================
FINAL SYSTEM OUTPUTS
====================================================================================================

Kernel registry:
    /proc/devices
        → 236 my_cdev

Sysfs representation:
    /sys/class/my_class/my_cdev0

User device node:
    /dev/my_cdev0

Runtime interface:
    open/read/write/ioctl routed through file_operations

====================================================================================================
```

*/