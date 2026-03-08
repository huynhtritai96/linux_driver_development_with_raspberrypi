obj-m += hello.o # pass to Kbuild system: List of object files to be built as loadable kernel modules
# -m: stand for module, tells the build system to build a loadable kernel module: for insmod and rmmod

KDIR := /lib/modules/$(shell uname -r)/build
PWD  := $(shell pwd)

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

# -C : Change to directory before executing the make command
# M= External module build directory
# src= Source directory of the module
# make -V : debug variable value
# make -n : print the commands that would be executed, without executing them
# make -j : run multiple jobs in parallel, speeding up the build process