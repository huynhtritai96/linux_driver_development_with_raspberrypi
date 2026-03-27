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
	{ 0x122c3a7e, "_printk" },
	{ 0x317df83, "kobject_put" },
	{ 0xbcab6ee6, "sscanf" },
	{ 0x3c3ff9fd, "sprintf" },
	{ 0xcc429b8f, "kernel_kobj" },
	{ 0xad811a0a, "kobject_create_and_add" },
	{ 0xe811d67, "sysfs_create_file_ns" },
	{ 0x474e54d2, "module_layout" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "5C42F6663C2D2DD01EAA247");
