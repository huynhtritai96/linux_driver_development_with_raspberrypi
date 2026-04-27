#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/mutex.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("MPCoding - LDD");
MODULE_DESCRIPTION("Our first character device");

static const char *my_device = "my_cdev";
static dev_t dev_nr;
static struct cdev my_cdev;
static struct class *my_class;

#define DEV_BUFFER_SIZE 64      /* Size of the device's internal buffer */

static char *dev_buffer;        /* Internal device buffer (kernel memory) and it's mutex */
static struct mutex dev_mutex;  // Mutex to protect access to the device buffer


// This function is called when the device file is opened. 
// It logs the major and minor numbers of the device, the file position, mode, and flags.
static int my_open(struct inode *pInode, struct file *pFile)
{
    pr_info("%s: my_open called, Major: %d Minor: %d\n"
            "%s: file->f_pos: %lld\n"
            "%s: file->f_mode: 0x%x\n"
            "%s: file->f_flags: 0x%x\n",
            my_device, imajor(pInode), iminor(pInode),
            my_device, pFile->f_pos,
            my_device, pFile->f_mode,
            my_device, pFile->f_flags);

    return 0;
}

// This function is called when the device file is closed. 
// It logs a message indicating that the file has been closed and returns 0 to indicate successful release.
static int my_release(struct inode *pInode, struct file *pFile)
{
    pr_info("%s: my_release called, file is closed\n", my_device);

    return 0;
}


/* read: copy data from kernel buffer -> user-space buffer*/
static ssize_t my_read(struct file *pFile, char __user *pUser_buff, size_t count, loff_t *pOffset)
{
    size_t bytes_to_copy, not_copied, copied;

    if (mutex_lock_interruptible(&dev_mutex))
        return -ERESTARTSYS;

    pr_info("%s: read called: request=%zu, offset=%lld, \n", my_device, count, *pOffset);
    
    // Calculate the number of bytes to copy, ensuring we don't read beyond the end of the valid data in dev_buffer. 
    bytes_to_copy = (count + *pOffset > strlen(dev_buffer)) ? (strlen(dev_buffer) - *pOffset) : count; 
    
    pr_info("%s: Read will copy=%zu bytes\n", my_device, bytes_to_copy);

    not_copied = copy_to_user(pUser_buff, dev_buffer + *pOffset, bytes_to_copy);    // Copy data from the kernel buffer (dev_buffer) to the user-space buffer (pUser_buff). 
                                                                                    // The copy starts from the offset specified by *pOffset in dev_buffer and attempts to copy bytes_to_copy bytes. 
                                                                                    // The function returns the number of bytes that could not be copied, which is stored in not_copied.
    copied = bytes_to_copy - not_copied; // Calculate the number of bytes that were successfully copied by subtracting not_copied from bytes_to_copy.

    *pOffset += copied;

    if (not_copied)
        pr_warn("%s: copy_to_user only copied %zu/%zu\n", my_device, copied, bytes_to_copy);

    pr_info("%s: Read done: return=%zu, new offset=%lld\n", my_device, copied, *pOffset);

    mutex_unlock(&dev_mutex);
    return (ssize_t)copied;
}


/* write: copy data from user-space buffer -> kernel buffer */
static ssize_t my_write(struct file *pFile, const char __user *pUser_buff, size_t count, loff_t *pOffset)
{
    size_t bytes_to_copy, not_copied, copied;

    if (mutex_lock_interruptible(&dev_mutex))   // Attempt to acquire the mutex lock (dev_mutex) to ensure exclusive access to the device buffer. 
                                                // If the lock is already held by another thread, the current thread will be put to sleep until the lock becomes available. 
                                                // If the thread is interrupted while waiting for the lock, it will return -ERESTARTSYS to indicate that the system call should be restarted.
        return -ERESTARTSYS;    // Return -ERESTARTSYS if the thread was interrupted while waiting for the mutex lock, 
                                // indicating that the system call should be restarted.
    
    if (*pOffset >= DEV_BUFFER_SIZE) // Check if the current file position (*pOffset) is greater than or equal to the size of the device buffer (DEV_BUFFER_SIZE). 
                                     // If it is, there is no space left to write, and the function returns -ENOSPC to indicate that there is no space left on the device.
    {
        pr_info("%s: no space left to write\n", my_device);
        mutex_unlock(&dev_mutex);
        return -ENOSPC;
    } 
    else if (count > DEV_BUFFER_SIZE - (size_t) *pOffset) // If the requested write size (count) exceeds the remaining space in the device buffer (DEV_BUFFER_SIZE - *pOffset),
    {
        bytes_to_copy = DEV_BUFFER_SIZE - (size_t) *pOffset;
        pr_info("%s: only %zu bytes left to write\n", my_device, bytes_to_copy);
    } 
    else
    {
        bytes_to_copy = count;
        pr_info("%s: writing %zu bytes\n", my_device, bytes_to_copy);
    }

    pr_info("%s: write request=%zu, offset=%lld, will copy=%zu\n", my_device, count, *pOffset, bytes_to_copy);

    not_copied = copy_from_user(dev_buffer + *pOffset, pUser_buff, bytes_to_copy);
    copied = bytes_to_copy - not_copied;

    if (not_copied)
    {
        pr_warn("%s: copy_from_user: only copied %zu/%zu\n", my_device, copied, bytes_to_copy);
    }

    *pOffset += copied;

    pr_info("%s: write done: copied=%zu, new offset=%lld\n", my_device, copied, *pOffset);

    mutex_unlock(&dev_mutex);
    return (size_t)copied;
}


/* file operations structure */
static struct file_operations fops = {
    .open = my_open,        // caller must pass arguments based on avaliable template in struct file_operations, otherwise it will cause compile error. 
                             // For example, if we define .open = my_open, then the my_open function must have the same signature as the open function pointer in struct file_operations, 
                             // which is int (*open) (struct inode *, struct file *). 
                             // If the signature does not match, it will result in a compile-time error.
    .release = my_release,
    .read = my_read,
    .write = my_write,
};


static int __init my_init(void)
{
    int status;

    /* allocate kernel buffer */
    dev_buffer = kmalloc(DEV_BUFFER_SIZE, GFP_KERNEL);  // Allocate kernel buffer for device data. 
                                                        // GFP_KERNEL flag indicates that the allocation is performed in the context of a process running in kernel space, 
                                                        // and it can sleep if necessary to wait for memory to become available.
    if (!dev_buffer)
    {
        pr_err("%s: Failed to allocate device buffer\n", my_device);
        return -ENOMEM;
    }

    mutex_init(&dev_mutex);                 /* initialize mutex */
    memset(dev_buffer, 0, DEV_BUFFER_SIZE); /* clear dev_buffer */

    status = alloc_chrdev_region(&dev_nr, 0, MINORMASK + 1, my_device);
    if (status)
    {
        pr_err("%s: character device registation failed\n", my_device);
        return status;
    }

    cdev_init(&my_cdev, &fops);     // Initialize the character device and link it with the file operations defined in fops.
    my_cdev.owner = THIS_MODULE;    // Set the owner of the character device to THIS_MODULE, which is a macro that points to the current kernel module.

    status = cdev_add(&my_cdev, dev_nr, MINORMASK + 1); // Add the character device to the system, making it available for use. 
                                                        // dev_nr specifies the major and minor numbers for the device, and MINORMASK + 1 indicates the number of minor numbers to allocate.
    if (status)
    {
        pr_err("%s: error adding cdev\n", my_device);
        goto free_device_nr;
    }

    my_class = class_create("my_class");        /* Create a device class named "my_class" in the sysfs filesystem. This class will be used to group the device nodes created for this character device. */
    if (!my_class)
    {
        pr_err("%s: Could not create class my_class\n",my_device);
        status = ENOMEM;
        goto delete_cdev;
    }

    if (!device_create(my_class, NULL, dev_nr, NULL, "my_cdev%d", 0))   // Create a device node named "my_cdev0" under the class "my_class" with the major and minor numbers specified in dev_nr. 
                                                                        // This makes the device accessible to user-space applications through the /dev/my_cdev0 file.
    {
        pr_err("%s: Could not create device my_cdev0\n", my_device);
        status = ENOMEM;
        goto delete_class;
    }

    pr_info("%s: Caracter device registerd, Major number: %d Minor number: %d\n",my_device, MAJOR(dev_nr), MINOR(dev_nr));
    pr_info("%s: Created device number under /sys/class/my_class\n", my_device);
    pr_info("%s: Created new device node /dev/my_cdev\n", my_device);
    return 0;

delete_class:
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
    mutex_destroy(&dev_mutex);
    pr_info("%s: Goodbye, Kernel\n", my_device);
}

module_init(my_init);
module_exit(my_exit);

/*
Terminal 1:
 $ sudo insmod my_cdev.ko
 $ ls -l /dev/my_cdev0 
crw------- 1 root root 236, 0 Mar 29 18:09 /dev/my_cdev0
 $ ls -l /sys/class/my_class/
total 0
lrwxrwxrwx 1 root root 0 Mar 29 18:09 my_cdev0 -> ../../devices/virtual/my_class/my_cdev0
 $ echo "hello world" | sudo tee /dev/my_cdev0 
hello world
 $ sudo cat /dev/my_cdev0 
hello world

Terminal 2:
$ dmesg -W
[28984.623521] my_cdev: my_open called, Major: 236 Minor: 0
               my_cdev: file->f_pos: 0
               my_cdev: file->f_mode: 0x1e
               my_cdev: file->f_flags: 0x20241
[28984.623587] my_cdev: writing 12 bytes
[28984.623591] my_cdev: write request=12, offset=0, will copy=12
[28984.623596] my_cdev: write done: copied=12, new offset=12
[28984.623606] my_cdev: my_release called, file is closed



[29033.888624] my_cdev: my_open called, Major: 236 Minor: 0
               my_cdev: file->f_pos: 0
               my_cdev: file->f_mode: 0x1d
               my_cdev: file->f_flags: 0x20000
[29033.888680] my_cdev: read called: request=131072, offset=0, 
[29033.888686] my_cdev: Read will copy=12 bytes
[29033.888694] my_cdev: Read done: return=12, new offset=12
[29033.888763] my_cdev: read called: request=131072, offset=12, 
[29033.888768] my_cdev: Read will copy=0 bytes
[29033.888771] my_cdev: Read done: return=0, new offset=12
[29033.888793] my_cdev: my_release called, file is closed
*/