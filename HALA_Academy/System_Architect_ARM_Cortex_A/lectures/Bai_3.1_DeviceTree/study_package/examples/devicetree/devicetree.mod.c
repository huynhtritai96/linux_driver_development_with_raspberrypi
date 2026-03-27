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
	{ 0xfa474811, "__platform_driver_register" },
	{ 0x36a78de3, "devm_kmalloc" },
	{ 0x3a1aed17, "of_property_read_string" },
	{ 0x80192c9b, "of_property_read_variable_u32_array" },
	{ 0x4cd6ab7e, "of_find_property" },
	{ 0xf0fdf6cb, "__stack_chk_fail" },
	{ 0x61fd46a9, "platform_driver_unregister" },
	{ 0x474e54d2, "module_layout" },
};

MODULE_INFO(depends, "");

MODULE_ALIAS("of:N*T*Clkmpg,example-device");
MODULE_ALIAS("of:N*T*Clkmpg,example-deviceC*");
MODULE_ALIAS("of:N*T*Clkmpg,another-device");
MODULE_ALIAS("of:N*T*Clkmpg,another-deviceC*");

MODULE_INFO(srcversion, "6660473FAED3E85AD89C4BA");
