# Example Index

## bh_threaded

- Example name: `bh_threaded`
- Path: `examples/bh_threaded.c`
- Topic: request_threaded_irq flow
- Type: kernel module
- Build command: `make -C /lib/modules/$(uname -r)/build M=<lkmpg-root>/examples bh_threaded.ko`
- Run command: `Blocked in this environment (GPIO/IRQ hardware unavailable).`
- Unload/cleanup command: `sudo rmmod bh_threaded (if loaded)`
- Expected privilege level: root
- Dependencies: kernel headers, sudo, button + LED GPIO wiring
- Runnable status: `needs_hardware`
- Notes: Threaded IRQ with IRQ_WAKE_THREAD.

## bottomhalf

- Example name: `bottomhalf`
- Path: `examples/bottomhalf.c`
- Topic: split top-half and workqueue bottom-half
- Type: kernel module
- Build command: `make -C /lib/modules/$(uname -r)/build M=<lkmpg-root>/examples bottomhalf.ko`
- Run command: `Blocked in this environment (GPIO/IRQ hardware unavailable).`
- Unload/cleanup command: `sudo rmmod bottomhalf (if loaded)`
- Expected privilege level: root
- Dependencies: kernel headers, sudo, button + LED GPIO wiring
- Runnable status: `needs_hardware`
- Notes: ISR schedules workqueue bottom half.

## cat_nonblock

- Example name: `cat_nonblock`
- Path: `examples/other/cat_nonblock.c`
- Topic: non-blocking userspace behavior
- Type: userspace helper
- Build command: `gcc -O2 -Wall -Wextra examples/other/cat_nonblock.c -o examples/other/cat_nonblock`
- Run command: `./examples/other/cat_nonblock /proc/sleep while sleep.ko holds open state`
- Unload/cleanup command: `rm -f examples/other/cat_nonblock`
- Expected privilege level: user (root not required for read)
- Dependencies: glibc toolchain, loaded sleep module
- Runnable status: `directly_runnable_with_module`
- Notes: Demonstrates O_NONBLOCK returning EAGAIN path.

## chardev

- Example name: `chardev`
- Path: `examples/chardev.c`
- Topic: file_operations and char device registration
- Type: kernel module
- Build command: `make -C /lib/modules/$(uname -r)/build M=<lkmpg-root>/examples chardev.ko`
- Run command: `sudo insmod examples/chardev.ko; sudo head -n 1 /dev/chardev; sudo rmmod chardev`
- Unload/cleanup command: `sudo rmmod chardev`
- Expected privilege level: root
- Dependencies: kernel headers, sudo, /dev/chardev
- Runnable status: `buildable_but_interactive`
- Notes: Read-only char device with open serialization.

## chardev2

- Example name: `chardev2`
- Path: `examples/chardev2.c`, `examples/chardev.h`
- Topic: kernel/userspace ioctl contract
- Type: kernel module
- Build command: `make -C /lib/modules/$(uname -r)/build M=<lkmpg-root>/examples chardev2.ko`
- Run command: `sudo insmod examples/chardev2.ko; sudo ./examples/other/userspace_ioctl; sudo rmmod chardev2`
- Unload/cleanup command: `sudo rmmod chardev2`
- Expected privilege level: root
- Dependencies: kernel headers, sudo, /dev/char_dev, shared ioctl header
- Runnable status: `buildable_but_interactive`
- Notes: Fixed major ioctl sample using shared header constants.

## completions

- Example name: `completions`
- Path: `examples/completions.c`
- Topic: one-shot synchronization points
- Type: kernel module
- Build command: `make -C /lib/modules/$(uname -r)/build M=<lkmpg-root>/examples completions.ko`
- Run command: `sudo insmod examples/completions.ko && sudo rmmod completions`
- Unload/cleanup command: `sudo rmmod completions`
- Expected privilege level: root
- Dependencies: kernel headers, sudo, kthread API
- Runnable status: `directly_runnable`
- Notes: Orders kthreads with completion primitives.

## devicemodel

- Example name: `devicemodel`
- Path: `examples/devicemodel.c`
- Topic: platform_driver lifecycle and PM callbacks
- Type: kernel module
- Build command: `make -C /lib/modules/$(uname -r)/build M=<lkmpg-root>/examples devicemodel.ko`
- Run command: `sudo insmod examples/devicemodel.ko && sudo rmmod devicemodel`
- Unload/cleanup command: `sudo rmmod devicemodel`
- Expected privilege level: root
- Dependencies: kernel headers, sudo, platform bus
- Runnable status: `partially_runnable`
- Notes: Platform driver registers/unregisters; probe requires matching device.

## devicetree

- Example name: `devicetree`
- Path: `examples/devicetree.c`
- Topic: of_match_table and DT property parsing
- Type: kernel module
- Build command: `make -C /lib/modules/$(uname -r)/build M=<lkmpg-root>/examples devicetree.ko`
- Run command: `sudo insmod examples/devicetree.ko && sudo rmmod devicetree`
- Unload/cleanup command: `sudo rmmod devicetree`
- Expected privilege level: root
- Dependencies: kernel headers, sudo, matching DT node for probe path
- Runnable status: `partially_runnable`
- Notes: Driver init/exit runs; probe depends on compatible DT node.

## dht11

- Example name: `dht11`
- Path: `examples/dht11.c`
- Topic: timing-sensitive sensor protocol in kernel space
- Type: kernel module
- Build command: `make -C /lib/modules/$(uname -r)/build M=<lkmpg-root>/examples dht11.ko`
- Run command: `Blocked in this environment (sensor/GPIO dependency).`
- Unload/cleanup command: `sudo rmmod dht11 (if loaded)`
- Expected privilege level: root
- Dependencies: kernel headers, sudo, DHT11 sensor wiring and timing
- Runnable status: `needs_hardware`
- Notes: Bit-banged DHT11 reading via GPIO transitions.

## dt-overlay

- Example name: `dt-overlay`
- Path: `examples/dt-overlay.dts`
- Topic: runtime device-tree overlay mechanics
- Type: device tree asset
- Build command: `dtc -@ -I dts -O dtb -o dt-overlay.dtbo dt-overlay.dts`
- Run command: `sudo dtoverlay dt-overlay.dtbo (board-dependent)`
- Unload/cleanup command: `sudo dtoverlay -r <index>`
- Expected privilege level: root
- Dependencies: device-tree compiler, platform overlay support
- Runnable status: `needs_board_support`
- Notes: Overlay sample for lkmpg,example-device compatibles.

## example_atomic

- Example name: `example_atomic`
- Path: `examples/example_atomic.c`
- Topic: lock-free scalar updates
- Type: kernel module
- Build command: `make -C /lib/modules/$(uname -r)/build M=<lkmpg-root>/examples example_atomic.ko`
- Run command: `sudo insmod examples/example_atomic.ko && sudo rmmod example_atomic`
- Unload/cleanup command: `sudo rmmod example_atomic`
- Expected privilege level: root
- Dependencies: kernel headers, sudo
- Runnable status: `directly_runnable`
- Notes: Atomic arithmetic and bit operations.

## example_mutex

- Example name: `example_mutex`
- Path: `examples/example_mutex.c`
- Topic: sleepable lock semantics
- Type: kernel module
- Build command: `make -C /lib/modules/$(uname -r)/build M=<lkmpg-root>/examples example_mutex.ko`
- Run command: `sudo insmod examples/example_mutex.ko && sudo rmmod example_mutex`
- Unload/cleanup command: `sudo rmmod example_mutex`
- Expected privilege level: root
- Dependencies: kernel headers, sudo
- Runnable status: `directly_runnable`
- Notes: Simple trylock/unlock flow.

## example_rwlock

- Example name: `example_rwlock`
- Path: `examples/example_rwlock.c`
- Topic: read-mostly lock strategy
- Type: kernel module
- Build command: `make -C /lib/modules/$(uname -r)/build M=<lkmpg-root>/examples example_rwlock.ko`
- Run command: `sudo insmod examples/example_rwlock.ko && sudo rmmod example_rwlock`
- Unload/cleanup command: `sudo rmmod example_rwlock`
- Expected privilege level: root
- Dependencies: kernel headers, sudo
- Runnable status: `directly_runnable`
- Notes: Reader/writer lock primitives.

## example_spinlock

- Example name: `example_spinlock`
- Path: `examples/example_spinlock.c`
- Topic: atomic-context locking
- Type: kernel module
- Build command: `make -C /lib/modules/$(uname -r)/build M=<lkmpg-root>/examples example_spinlock.ko`
- Run command: `sudo insmod examples/example_spinlock.ko && sudo rmmod example_spinlock`
- Unload/cleanup command: `sudo rmmod example_spinlock`
- Expected privilege level: root
- Dependencies: kernel headers, sudo
- Runnable status: `directly_runnable`
- Notes: Demonstrates irqsave spinlock variants.

## example_tasklet

- Example name: `example_tasklet`
- Path: `examples/example_tasklet.c`
- Topic: softirq-based deferred work
- Type: kernel module
- Build command: `make -C /lib/modules/$(uname -r)/build M=<lkmpg-root>/examples example_tasklet.ko`
- Run command: `sudo insmod examples/example_tasklet.ko && sudo rmmod example_tasklet`
- Unload/cleanup command: `sudo rmmod example_tasklet`
- Expected privilege level: root
- Dependencies: kernel headers, sudo
- Runnable status: `directly_runnable`
- Notes: Deferred execution with tasklet scheduling.

## hello-1

- Example name: `hello-1`
- Path: `examples/hello-1.c`
- Topic: module lifecycle basics
- Type: kernel module
- Build command: `make -C /lib/modules/$(uname -r)/build M=<lkmpg-root>/examples hello-1.ko`
- Run command: `sudo insmod examples/hello-1.ko && sudo rmmod hello-1`
- Unload/cleanup command: `sudo rmmod hello-1`
- Expected privilege level: root
- Dependencies: kernel headers, sudo
- Runnable status: `directly_runnable`
- Notes: Minimal module using init_module/cleanup_module.

## hello-2

- Example name: `hello-2`
- Path: `examples/hello-2.c`
- Topic: preferred init/exit registration
- Type: kernel module
- Build command: `make -C /lib/modules/$(uname -r)/build M=<lkmpg-root>/examples hello-2.ko`
- Run command: `sudo insmod examples/hello-2.ko && sudo rmmod hello-2`
- Unload/cleanup command: `sudo rmmod hello-2`
- Expected privilege level: root
- Dependencies: kernel headers, sudo
- Runnable status: `directly_runnable`
- Notes: Uses module_init/module_exit macros.

## hello-3

- Example name: `hello-3`
- Path: `examples/hello-3.c`
- Topic: init/exit section annotations
- Type: kernel module
- Build command: `make -C /lib/modules/$(uname -r)/build M=<lkmpg-root>/examples hello-3.ko`
- Run command: `sudo insmod examples/hello-3.ko && sudo rmmod hello-3`
- Unload/cleanup command: `sudo rmmod hello-3`
- Expected privilege level: root
- Dependencies: kernel headers, sudo
- Runnable status: `directly_runnable`
- Notes: Adds __init/__exit annotations.

## hello-4

- Example name: `hello-4`
- Path: `examples/hello-4.c`
- Topic: module metadata and licensing
- Type: kernel module
- Build command: `make -C /lib/modules/$(uname -r)/build M=<lkmpg-root>/examples hello-4.ko`
- Run command: `sudo insmod examples/hello-4.ko && sudo rmmod hello-4`
- Unload/cleanup command: `sudo rmmod hello-4`
- Expected privilege level: root
- Dependencies: kernel headers, sudo
- Runnable status: `directly_runnable`
- Notes: Adds module metadata fields.

## hello-5

- Example name: `hello-5`
- Path: `examples/hello-5.c`
- Topic: module_param interfaces
- Type: kernel module
- Build command: `make -C /lib/modules/$(uname -r)/build M=<lkmpg-root>/examples hello-5.ko`
- Run command: `sudo insmod examples/hello-5.ko && sudo rmmod hello-5`
- Unload/cleanup command: `sudo rmmod hello-5`
- Expected privilege level: root
- Dependencies: kernel headers, sudo
- Runnable status: `directly_runnable`
- Notes: Demonstrates module parameters.

## hello-sysfs

- Example name: `hello-sysfs`
- Path: `examples/hello-sysfs.c`
- Topic: sysfs attribute plumbing
- Type: kernel module
- Build command: `make -C /lib/modules/$(uname -r)/build M=<lkmpg-root>/examples hello-sysfs.ko`
- Run command: `sudo insmod examples/hello-sysfs.ko; sudo cat /sys/kernel/mymodule/myvariable; echo 456 | sudo tee /sys/kernel/mymodule/myvariable; sudo rmmod hello-sysfs`
- Unload/cleanup command: `sudo rmmod hello-sysfs`
- Expected privilege level: root for writes and module operations
- Dependencies: kernel headers, sudo, sysfs mounted
- Runnable status: `directly_runnable`
- Notes: kobject + sysfs attribute show/store flow.

## intrpt

- Example name: `intrpt`
- Path: `examples/intrpt.c`
- Topic: top-half IRQ callback
- Type: kernel module
- Build command: `make -C /lib/modules/$(uname -r)/build M=<lkmpg-root>/examples intrpt.ko`
- Run command: `Blocked in this environment (GPIO/IRQ hardware unavailable).`
- Unload/cleanup command: `sudo rmmod intrpt (if loaded)`
- Expected privilege level: root
- Dependencies: kernel headers, sudo, button + LED GPIO wiring
- Runnable status: `needs_hardware`
- Notes: GPIO IRQ handling toggling LED state.

## ioctl

- Example name: `ioctl`
- Path: `examples/ioctl.c`
- Topic: ioctl ABI surface and locking
- Type: kernel module
- Build command: `make -C /lib/modules/$(uname -r)/build M=<lkmpg-root>/examples ioctl.ko`
- Run command: `sudo insmod examples/ioctl.ko; create temp node from /proc/devices; read bytes; sudo rmmod ioctl`
- Unload/cleanup command: `sudo rmmod ioctl && sudo rm -f /tmp/ioctltest`
- Expected privilege level: root
- Dependencies: kernel headers, sudo, manual device-node creation
- Runnable status: `buildable_but_interactive`
- Notes: Dynamic major with ioctl command handling and rwlock.

## kbleds

- Example name: `kbleds`
- Path: `examples/kbleds.c`
- Topic: timer callbacks + tty ioctl integration
- Type: kernel module
- Build command: `make -C /lib/modules/$(uname -r)/build M=<lkmpg-root>/examples kbleds.ko`
- Run command: `sudo insmod examples/kbleds.ko; sleep 1; sudo rmmod kbleds`
- Unload/cleanup command: `sudo rmmod kbleds`
- Expected privilege level: root
- Dependencies: kernel headers, sudo, virtual console/tty stack
- Runnable status: `directly_runnable`
- Notes: Timer-driven keyboard LED ioctl toggling.

## led

- Example name: `led`
- Path: `examples/led.c`
- Topic: GPIO output via char device writes
- Type: kernel module
- Build command: `make -C /lib/modules/$(uname -r)/build M=<lkmpg-root>/examples led.ko`
- Run command: `Blocked in this environment (GPIO request failure).`
- Unload/cleanup command: `sudo rmmod led (if loaded)`
- Expected privilege level: root
- Dependencies: kernel headers, sudo, GPIO line ownership and wiring
- Runnable status: `needs_hardware`
- Notes: Character device controlling GPIO LED state.

## print_string

- Example name: `print_string`
- Path: `examples/print_string.c`
- Topic: tty write path from kernel
- Type: kernel module
- Build command: `make -C /lib/modules/$(uname -r)/build M=<lkmpg-root>/examples print_string.ko`
- Run command: `sudo insmod examples/print_string.ko && sudo rmmod print_string`
- Unload/cleanup command: `sudo rmmod print_string`
- Expected privilege level: root
- Dependencies: kernel headers, sudo, active tty context
- Runnable status: `directly_runnable`
- Notes: Writes directly to current task tty driver ops.

## procfs1

- Example name: `procfs1`
- Path: `examples/procfs1.c`
- Topic: proc_create + read callback
- Type: kernel module
- Build command: `make -C /lib/modules/$(uname -r)/build M=<lkmpg-root>/examples procfs1.ko`
- Run command: `sudo insmod examples/procfs1.ko; cat /proc/helloworld; sudo rmmod procfs1`
- Unload/cleanup command: `sudo rmmod procfs1`
- Expected privilege level: root for module operations
- Dependencies: kernel headers, sudo, /proc
- Runnable status: `directly_runnable`
- Notes: Minimal /proc read endpoint.

## procfs2

- Example name: `procfs2`
- Path: `examples/procfs2.c`
- Topic: copy_from_user/copy_to_user via procfs
- Type: kernel module
- Build command: `make -C /lib/modules/$(uname -r)/build M=<lkmpg-root>/examples procfs2.ko`
- Run command: `sudo insmod examples/procfs2.ko; echo text | sudo tee /proc/buffer1k; cat /proc/buffer1k; sudo rmmod procfs2`
- Unload/cleanup command: `sudo rmmod procfs2`
- Expected privilege level: root for write path and module ops
- Dependencies: kernel headers, sudo, /proc
- Runnable status: `directly_runnable`
- Notes: Adds proc write callback and bounded buffer.

## procfs3

- Example name: `procfs3`
- Path: `examples/procfs3.c`
- Topic: proc file lifecycle and ownership metadata
- Type: kernel module
- Build command: `make -C /lib/modules/$(uname -r)/build M=<lkmpg-root>/examples procfs3.ko`
- Run command: `sudo insmod examples/procfs3.ko; echo text | sudo tee /proc/buffer2k; cat /proc/buffer2k; sudo rmmod procfs3`
- Unload/cleanup command: `sudo rmmod procfs3`
- Expected privilege level: root for write path and module ops
- Dependencies: kernel headers, sudo, /proc
- Runnable status: `directly_runnable`
- Notes: Shows standard filesystem-style handlers.

## procfs4

- Example name: `procfs4`
- Path: `examples/procfs4.c`
- Topic: seq_file iteration
- Type: kernel module
- Build command: `make -C /lib/modules/$(uname -r)/build M=<lkmpg-root>/examples procfs4.ko`
- Run command: `sudo insmod examples/procfs4.ko; cat /proc/iter; cat /proc/iter; sudo rmmod procfs4`
- Unload/cleanup command: `sudo rmmod procfs4`
- Expected privilege level: root for module operations
- Dependencies: kernel headers, sudo, seq_file API
- Runnable status: `directly_runnable`
- Notes: seq_file iterator sample.

## sched

- Example name: `sched`
- Path: `examples/sched.c`
- Topic: workqueue-based deferred execution
- Type: kernel module
- Build command: `make -C /lib/modules/$(uname -r)/build M=<lkmpg-root>/examples sched.ko`
- Run command: `sudo insmod examples/sched.ko && sudo rmmod sched`
- Unload/cleanup command: `sudo rmmod sched`
- Expected privilege level: root
- Dependencies: kernel headers, sudo, workqueue API
- Runnable status: `directly_runnable`
- Notes: Allocates custom workqueue and queues work item.

## sleep

- Example name: `sleep`
- Path: `examples/sleep.c`
- Topic: blocking semantics and wakeups
- Type: kernel module
- Build command: `make -C /lib/modules/$(uname -r)/build M=<lkmpg-root>/examples sleep.ko`
- Run command: `sudo insmod examples/sleep.ko; echo napmsg | sudo tee /proc/sleep; cat /proc/sleep; sudo rmmod sleep`
- Unload/cleanup command: `sudo rmmod sleep`
- Expected privilege level: root for module ops and writes
- Dependencies: kernel headers, sudo, /proc/sleep
- Runnable status: `directly_runnable`
- Notes: Wait queue and non-blocking open behavior.

## startstop

- Example name: `startstop`
- Path: `examples/start.c`, `examples/stop.c`
- Topic: modular compilation units
- Type: kernel module (multi-file)
- Build command: `make -C /lib/modules/$(uname -r)/build M=<lkmpg-root>/examples startstop.ko`
- Run command: `sudo insmod examples/startstop.ko && sudo rmmod startstop`
- Unload/cleanup command: `sudo rmmod startstop`
- Expected privilege level: root
- Dependencies: kernel headers, sudo, examples/Makefile startstop-objs
- Runnable status: `directly_runnable`
- Notes: Multi-object module linked by Kbuild.

## static_key

- Example name: `static_key`
- Path: `examples/static_key.c`
- Topic: jump labels and static_branch_{enable,disable}
- Type: kernel module
- Build command: `make -C /lib/modules/$(uname -r)/build M=<lkmpg-root>/examples static_key.ko`
- Run command: `sudo insmod examples/static_key.ko; read/write /dev/key_state; sudo rmmod static_key`
- Unload/cleanup command: `sudo rmmod static_key`
- Expected privilege level: root
- Dependencies: kernel headers, sudo, jump label support
- Runnable status: `directly_runnable`
- Notes: Runtime toggle of static branch via character device command.

## syscall-steal

- Example name: `syscall-steal`
- Path: `examples/syscall-steal.c`
- Topic: syscall table hooking risks
- Type: kernel module
- Build command: `make -C /lib/modules/$(uname -r)/build M=<lkmpg-root>/examples syscall-steal.ko`
- Run command: `Not runnable on this host due build failure.`
- Unload/cleanup command: `n/a`
- Expected privilege level: root when runnable
- Dependencies: architecture-specific internals, sys_call_table strategy
- Runnable status: `build_fail_arch_specific`
- Notes: Fails on ARM64 here (pt_regs member mismatch + x86 write-protect assumptions).

## userspace_ioctl

- Example name: `userspace_ioctl`
- Path: `examples/other/userspace_ioctl.c`, `examples/chardev.h`
- Topic: userspace ioctl client flow
- Type: userspace helper
- Build command: `gcc -O2 -Wall -Wextra examples/other/userspace_ioctl.c -o examples/other/userspace_ioctl`
- Run command: `sudo ./examples/other/userspace_ioctl (after chardev2 is loaded)`
- Unload/cleanup command: `rm -f examples/other/userspace_ioctl`
- Expected privilege level: root (for /dev/char_dev access in this environment)
- Dependencies: glibc toolchain, loaded chardev2 module
- Runnable status: `directly_runnable_with_module`
- Notes: Exercises IOCTL_SET_MSG/GET_MSG/GET_NTH_BYTE operations.

## vinput

- Example name: `vinput`
- Path: `examples/vinput.c`, `examples/vinput.h`
- Topic: registering virtual input framework and class attributes
- Type: kernel module
- Build command: `make -C /lib/modules/$(uname -r)/build M=<lkmpg-root>/examples vinput.ko`
- Run command: `sudo insmod examples/vinput.ko; ls /sys/class/vinput; sudo rmmod vinput`
- Unload/cleanup command: `sudo rmmod vinput`
- Expected privilege level: root
- Dependencies: kernel headers, sudo, input subsystem
- Runnable status: `directly_runnable`
- Notes: Virtual input core exposing export/unexport sysfs attrs.

## vkbd

- Example name: `vkbd`
- Path: `examples/vkbd.c`, `examples/vinput.h`, `examples/vinput.c`
- Topic: cross-module symbol dependency in out-of-tree modules
- Type: kernel module
- Build command: `make -C /lib/modules/$(uname -r)/build M=<lkmpg-root>/examples KBUILD_EXTRA_SYMBOLS=<lkmpg-root>/examples/Module.symvers vkbd.ko`
- Run command: `Build vkbd with vinput symbols, load vinput then vkbd, export vkbd device via /sys/class/vinput/export`
- Unload/cleanup command: `sudo rmmod vkbd && sudo rmmod vinput`
- Expected privilege level: root
- Dependencies: kernel headers, sudo, vinput symbols from Module.symvers
- Runnable status: `runnable_with_build_adaptation`
- Notes: Requires KBUILD_EXTRA_SYMBOLS and loaded vinput module.
