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

KSYMTAB_FUNC(vinput_register, "", "");
KSYMTAB_FUNC(vinput_unregister, "", "");

SYMBOL_CRC(vinput_register, 0xaaa2ae57, "");
SYMBOL_CRC(vinput_unregister, 0x58b2a9bb, "");

static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0xba8fbd64, "_raw_spin_lock" },
	{ 0xb5b54b34, "_raw_spin_unlock" },
	{ 0x122c3a7e, "_printk" },
	{ 0x3854774b, "kstrtoll" },
	{ 0x32a0c53, "input_unregister_device" },
	{ 0x3a7abb82, "device_unregister" },
	{ 0xf0fdf6cb, "__stack_chk_fail" },
	{ 0xa65c6def, "alt_cb_patch_nops" },
	{ 0x37a0cba, "kfree" },
	{ 0xdcb764ad, "memset" },
	{ 0x12a4e128, "__arch_copy_from_user" },
	{ 0x18175edc, "_dev_warn" },
	{ 0x6cbbfc54, "__arch_copy_to_user" },
	{ 0x418c10ec, "__register_chrdev" },
	{ 0xa42d469b, "class_register" },
	{ 0x6bc3fbc0, "__unregister_chrdev" },
	{ 0x7143a676, "class_unregister" },
	{ 0x98cf60b3, "strlen" },
	{ 0x5a921311, "strncmp" },
	{ 0x4fc61f86, "kmalloc_caches" },
	{ 0x5443de3e, "__kmalloc_cache_noprof" },
	{ 0x99c44b73, "input_allocate_device" },
	{ 0x5d71b8c5, "dev_set_name" },
	{ 0x7867bdc9, "device_register" },
	{ 0xb727b479, "input_free_device" },
	{ 0xe67106b1, "_dev_info" },
	{ 0x474e54d2, "module_layout" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "723E089A904738089C640D1");
