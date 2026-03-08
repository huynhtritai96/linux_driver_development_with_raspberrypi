#include <linux/module.h>
#include <linux/init.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("MPCoding - LDD");
MODULE_DESCRIPTION("Our first dynamically loadable kernel module");

static int __init my_init(void) {
    printk("Hello: Hello, Kernel\n");
    return 0;
}

static void __exit my_exit(void) {
    printk("Hello: Goodbye, Kernel\n");
}

module_init(my_init);
module_exit(my_exit);


/*
*.ko: kernel object 
insmod: insert module into kernel
    relocatable BLOB of compilied code with all symbols and deaths resolved, ready to be loaded into the kernel: 
    running kernel space without rebooting the system 
rmmod: remove module from kernel
lsmod: list loaded modules in kernel
dmesg: print kernel messages, used to check the output of printk in the kernel module
dmesg -W: wait for new kernel messages, useful for real-time monitoring of kernel messages when inserting or removing modules
    == sudo dmesg | tail -f -n 25 : alternative command to monitor kernel messages in real-time
*/