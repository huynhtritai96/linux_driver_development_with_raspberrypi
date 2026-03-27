#include <linux/module.h>
#include <linux/export-internal.h>
#include <linux/compiler.h>

MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};



static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0x25974000, "wait_for_completion" },
	{ 0x122c3a7e, "_printk" },
	{ 0x93d6dd8c, "complete_all" },
	{ 0x6008689f, "kthread_complete_and_exit" },
	{ 0x608741b5, "__init_swait_queue_head" },
	{ 0xcb661d89, "kthread_create_on_node" },
	{ 0x20bc8744, "wake_up_process" },
	{ 0xd45b3bc6, "kthread_stop" },
	{ 0x474e54d2, "module_layout" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "A86692770DFEFCF09EC56EA");
