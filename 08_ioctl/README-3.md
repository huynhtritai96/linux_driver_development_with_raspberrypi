$ make
make -C /lib/modules/6.12.47+rpt-rpi-v8/build M=/home/htritai/ldd/linux_driver_development_with_raspberrypi/08_ioctl modules
make[1]: Entering directory '/usr/src/linux-headers-6.12.47+rpt-rpi-v8'
  CC [M]  /home/htritai/ldd/linux_driver_development_with_raspberrypi/08_ioctl/my_cdev.o
  MODPOST /home/htritai/ldd/linux_driver_development_with_raspberrypi/08_ioctl/Module.symvers
  CC [M]  /home/htritai/ldd/linux_driver_development_with_raspberrypi/08_ioctl/my_cdev.mod.o
  CC [M]  /home/htritai/ldd/linux_driver_development_with_raspberrypi/08_ioctl/.module-common.o
  LD [M]  /home/htritai/ldd/linux_driver_development_with_raspberrypi/08_ioctl/my_cdev.ko
make[1]: Leaving directory '/usr/src/linux-headers-6.12.47+rpt-rpi-v8'
gcc test1.c


******************** Run log





















