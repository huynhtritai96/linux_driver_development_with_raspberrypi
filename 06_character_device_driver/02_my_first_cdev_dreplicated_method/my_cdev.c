#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Example");
MODULE_DESCRIPTION("Simple Character Device");

static const char *my_device = "my_cdev";

#define MY_MAJOR 64

static struct file_operations fops = {
};

static int __init my_init(void)
{
    int status;

    status = register_chrdev(MY_MAJOR, my_device, &fops);

    if (status) {
        pr_err("%s: character device registration failed\n", my_device);
        return status;
    }

    pr_info("%s: Character device registered, Major number: %d\n",
            my_device, MY_MAJOR);

    return 0;
}

static void __exit my_exit(void)
{
    unregister_chrdev(MY_MAJOR, my_device);
    pr_info("%s: Goodbye, Kernel\n", my_device);
}

module_init(my_init);
module_exit(my_exit);