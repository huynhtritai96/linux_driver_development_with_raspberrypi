# TOPIC SUMMARY MAP

| Topic folder | Topic | Matched source files | Confidence | Notes |
|---|---|---|---|---|
| `topics/01_hello_world` | Hello World | `examples/hello-1.c`, `examples/hello-2.c`, `examples/hello-3.c`, `examples/hello-4.c`, `examples/hello-5.c`, `examples/start.c`, `examples/stop.c` | `high` | direct chapter-to-source mapping from lkmpg.tex references |
| `topics/02_character_device_drivers` | Character Device drivers | `examples/chardev.c` | `high` | direct chapter-to-source mapping from lkmpg.tex references |
| `topics/03_proc_filesystem` | The /proc Filesystem | `examples/procfs1.c`, `examples/procfs2.c`, `examples/procfs3.c`, `examples/procfs4.c` | `high` | direct chapter-to-source mapping from lkmpg.tex references |
| `topics/04_sysfs_interacting_with_your_module` | sysfs: Interacting with your module | `examples/hello-sysfs.c` | `high` | direct chapter-to-source mapping from lkmpg.tex references |
| `topics/05_talking_to_device_files` | Talking To Device Files | `examples/chardev.h`, `examples/chardev2.c`, `examples/ioctl.c`, `examples/other/userspace_ioctl.c` | `high` | direct chapter-to-source mapping from lkmpg.tex references |
| `topics/06_system_calls` | System Calls | `examples/syscall-steal.c` | `medium` | syscall-steal:build_fail_arch_specific |
| `topics/07_blocking_processes_and_threads` | Blocking Processes and threads | `examples/completions.c`, `examples/other/cat_nonblock.c`, `examples/sleep.c` | `high` | direct chapter-to-source mapping from lkmpg.tex references |
| `topics/08_synchronization` | Synchronization | `examples/example_atomic.c`, `examples/example_mutex.c`, `examples/example_rwlock.c`, `examples/example_spinlock.c` | `high` | direct chapter-to-source mapping from lkmpg.tex references |
| `topics/09_replacing_print_macros` | Replacing Print Macros | `examples/kbleds.c`, `examples/print_string.c` | `high` | direct chapter-to-source mapping from lkmpg.tex references |
| `topics/10_gpio` | GPIO | `examples/dht11.c`, `examples/led.c` | `medium` | led:needs_hardware; dht11:needs_hardware |
| `topics/11_scheduling_tasks` | Scheduling Tasks | `examples/example_tasklet.c`, `examples/sched.c` | `high` | direct chapter-to-source mapping from lkmpg.tex references |
| `topics/12_interrupt_handlers` | Interrupt Handlers | `examples/bh_threaded.c`, `examples/bottomhalf.c`, `examples/intrpt.c` | `medium` | intrpt:needs_hardware; bottomhalf:needs_hardware; bh_threaded:needs_hardware |
| `topics/13_virtual_input_device_driver` | Virtual Input Device Driver | `examples/vinput.c`, `examples/vinput.h`, `examples/vkbd.c` | `medium` | vkbd:runnable_with_build_adaptation |
| `topics/14_device_model` | Standardizing the interfaces: The Device Model | `examples/devicemodel.c` | `medium` | devicemodel:partially_runnable |
| `topics/15_device_tree` | Device Tree | `examples/devicetree.c`, `examples/dt-overlay.dts` | `medium` | devicetree:partially_runnable; dt-overlay:needs_board_support |
| `topics/16_optimizations` | Optimizations | `examples/static_key.c` | `high` | direct chapter-to-source mapping from lkmpg.tex references |
