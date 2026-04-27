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
	{ 0x37a0cba, "kfree" },
	{ 0x999e8297, "vfree" },
	{ 0x9f625b5c, "kmem_cache_free" },
	{ 0x57a4a3ba, "kmem_cache_destroy" },
	{ 0x122c3a7e, "_printk" },
	{ 0x4fc61f86, "kmalloc_caches" },
	{ 0x5443de3e, "__kmalloc_cache_noprof" },
	{ 0xe4bbc1dd, "kimage_voffset" },
	{ 0x1b2fa1a9, "vmalloc_noprof" },
	{ 0x67083f6d, "__kmem_cache_create_args" },
	{ 0x51f9be39, "kmem_cache_alloc_noprof" },
	{ 0x9688de8b, "memstart_addr" },
	{ 0xf0fdf6cb, "__stack_chk_fail" },
	{ 0x474e54d2, "module_layout" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "5C4A2529217E97BC15ADC41");
