# COURSE SUMMARY

## Aggregate totals
- Topic folders processed: 16
- Total examples found: 38
- Total runnable examples created: 31
- Total examples actually executed (successful run logs): 31
- Total expected-output-only examples: 7

## Topic folders processed
| Topic folder | Topic title | Documentation files found | Source files found | # examples found | # runnable examples created | # actually executed | # expected-output-only |
|---|---|---|---|---:|---:|---:|---:|
| `topics/01_hello_world` | Hello World | `README.md` | `examples/hello-1.c`, `examples/hello-2.c`, `examples/hello-3.c`, `examples/hello-4.c`, `examples/hello-5.c`, `examples/start.c`, `examples/stop.c` | 6 | 6 | 6 | 0 |
| `topics/02_character_device_drivers` | Character Device drivers | `examples/chardev.c` | `examples/chardev.c` | 1 | 1 | 1 | 0 |
| `topics/03_proc_filesystem` | The /proc Filesystem | `examples/procfs1.c`, `examples/procfs2.c`, `examples/procfs3.c`, `examples/procfs4.c` | `examples/procfs1.c`, `examples/procfs2.c`, `examples/procfs3.c`, `examples/procfs4.c` | 4 | 4 | 4 | 0 |
| `topics/04_sysfs_interacting_with_your_module` | sysfs: Interacting with your module | `examples/hello-sysfs.c` | `examples/hello-sysfs.c` | 1 | 1 | 1 | 0 |
| `topics/05_talking_to_device_files` | Talking To Device Files | `examples/ioctl.c`, `examples/chardev2.c`, `examples/chardev.h`, `examples/other/userspace_ioctl.c` | `examples/chardev.h`, `examples/chardev2.c`, `examples/ioctl.c`, `examples/other/userspace_ioctl.c` | 3 | 3 | 3 | 0 |
| `topics/06_system_calls` | System Calls | `examples/syscall-steal.c` | `examples/syscall-steal.c` | 1 | 0 | 0 | 1 |
| `topics/07_blocking_processes_and_threads` | Blocking Processes and threads | `examples/sleep.c`, `examples/completions.c`, `examples/other/cat_nonblock.c` | `examples/completions.c`, `examples/other/cat_nonblock.c`, `examples/sleep.c` | 3 | 3 | 3 | 0 |
| `topics/08_synchronization` | Synchronization | `examples/example_mutex.c`, `examples/example_spinlock.c`, `examples/example_rwlock.c`, `examples/example_atomic.c` | `examples/example_atomic.c`, `examples/example_mutex.c`, `examples/example_rwlock.c`, `examples/example_spinlock.c` | 4 | 4 | 4 | 0 |
| `topics/09_replacing_print_macros` | Replacing Print Macros | `examples/print_string.c`, `examples/kbleds.c` | `examples/kbleds.c`, `examples/print_string.c` | 2 | 2 | 2 | 0 |
| `topics/10_gpio` | GPIO | `examples/led.c`, `examples/dht11.c` | `examples/dht11.c`, `examples/led.c` | 2 | 0 | 0 | 2 |
| `topics/11_scheduling_tasks` | Scheduling Tasks | `examples/example_tasklet.c`, `examples/sched.c` | `examples/example_tasklet.c`, `examples/sched.c` | 2 | 2 | 2 | 0 |
| `topics/12_interrupt_handlers` | Interrupt Handlers | `examples/intrpt.c`, `examples/bottomhalf.c`, `examples/bh_threaded.c` | `examples/bh_threaded.c`, `examples/bottomhalf.c`, `examples/intrpt.c` | 3 | 0 | 0 | 3 |
| `topics/13_virtual_input_device_driver` | Virtual Input Device Driver | `examples/vinput.c`, `examples/vkbd.c`, `examples/vinput.h` | `examples/vinput.c`, `examples/vinput.h`, `examples/vkbd.c` | 2 | 2 | 2 | 0 |
| `topics/14_device_model` | Standardizing the interfaces: The Device Model | `examples/devicemodel.c` | `examples/devicemodel.c` | 1 | 1 | 1 | 0 |
| `topics/15_device_tree` | Device Tree | `examples/devicetree.c`, `examples/dt-overlay.dts` | `examples/devicetree.c`, `examples/dt-overlay.dts` | 2 | 1 | 1 | 1 |
| `topics/16_optimizations` | Optimizations | `examples/static_key.c` | `examples/static_key.c` | 1 | 1 | 1 | 0 |

## Topic folders needing manual review
- `topics/06_system_calls` (build_fail_arch_specific)
- `topics/10_gpio` (needs_hardware)
- `topics/12_interrupt_handlers` (needs_hardware)
- `topics/13_virtual_input_device_driver` (runnable_with_build_adaptation)
- `topics/14_device_model` (partially_runnable)
- `topics/15_device_tree` (needs_board_support, partially_runnable)

## Major dependencies or blockers
- Kernel headers must match the running kernel (`/lib/modules/$(uname -r)/build`).
- Root privileges are required for module insertion/removal and privileged interface writes.
- GPIO/IRQ/sensor topics need board-specific hardware and pin availability.
- `syscall-steal` is architecture-sensitive and does not build on this ARM64 host.
- `vkbd` requires symbol linkage from `vinput` via `KBUILD_EXTRA_SYMBOLS`.
