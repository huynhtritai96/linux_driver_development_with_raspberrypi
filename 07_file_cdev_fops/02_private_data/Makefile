obj-m += my_cdev.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	gcc -o test1 test1.c

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm -f test1