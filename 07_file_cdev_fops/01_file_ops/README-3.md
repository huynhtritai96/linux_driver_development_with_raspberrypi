
```bash
$ make
make -C /lib/modules/6.12.47+rpt-rpi-v8/build M=/home/htritai/ldd/linux_driver_development_with_raspberrypi/07_file_cdev_fops/01_file_ops modules
make[1]: Entering directory '/usr/src/linux-headers-6.12.47+rpt-rpi-v8'
  CC [M]  /home/htritai/ldd/linux_driver_development_with_raspberrypi/07_file_cdev_fops/01_file_ops/my_cdev.o
  MODPOST /home/htritai/ldd/linux_driver_development_with_raspberrypi/07_file_cdev_fops/01_file_ops/Module.symvers
  CC [M]  /home/htritai/ldd/linux_driver_development_with_raspberrypi/07_file_cdev_fops/01_file_ops/my_cdev.mod.o
  CC [M]  /home/htritai/ldd/linux_driver_development_with_raspberrypi/07_file_cdev_fops/01_file_ops/.module-common.o
  LD [M]  /home/htritai/ldd/linux_driver_development_with_raspberrypi/07_file_cdev_fops/01_file_ops/my_cdev.ko
make[1]: Leaving directory '/usr/src/linux-headers-6.12.47+rpt-rpi-v8'
gcc -o test1 test1.c
```
 $ ls /dev
```
htritai@Rasp26:~/ldd/linux_driver_development_with_raspberrypi/07_file_cdev_fops/01_file_ops $ ls /dev
autofs           gpiochip4  loop-control  ram1     shm     tty2   tty36  tty52      udmabuf  vcsa2        vhost-vsock
block            gpiomem    mapper        ram10    snd     tty20  tty37  tty53      uhid     vcsa3        video10
btrfs-control    hwrng      media0        ram11    stderr  tty21  tty38  tty54      uinput   vcsa4        video11
bus              i2c-20     media1        ram12    stdin   tty22  tty39  tty55      urandom  vcsa5        video12
cec0             i2c-21     media2        ram13    stdout  tty23  tty4   tty56      v4l      vcsa6        video13
cec1             initctl    media3        ram14    tty     tty24  tty40  tty57      vchiq    vcsa7        video14
char             input      mem           ram15    tty0    tty25  tty41  tty58      vcio     vcsm-cma     video15
console          kmsg       mmcblk0       ram2     tty1    tty26  tty42  tty59      vc-mem   vcsu         video16
cpu_dma_latency  kvm        mmcblk0p1     ram3     tty10   tty27  tty43  tty6       vcs      vcsu1        video18
cuse             log        mmcblk0p2     ram4     tty11   tty28  tty44  tty60      vcs1     vcsu2        video19
disk             loop0      mqueue        ram5     tty12   tty29  tty45  tty61      vcs2     vcsu3        video20
dma_heap         loop1      net           ram6     tty13   tty3   tty46  tty62      vcs3     vcsu4        video21
dri              loop2      null          ram7     tty14   tty30  tty47  tty63      vcs4     vcsu5        video22
fd               loop3      port          ram8     tty15   tty31  tty48  tty7       vcs5     vcsu6        video23
full             loop4      ppp           ram9     tty16   tty32  tty49  tty8       vcs6     vcsu7        video31
fuse             loop5      ptmx          random   tty17   tty33  tty5   tty9       vcs7     vga_arbiter  watchdog
gpiochip0        loop6      pts           rfkill   tty18   tty34  tty50  ttyprintk  vcsa     vhci         watchdog0
gpiochip1        loop7      ram0          serial0  tty19   tty35  tty51  ttyS0      vcsa1    vhost-net    zero
htritai@Rasp26:~/ldd/linux_driver_development_with_raspberrypi/07_file_cdev_fops/01_file_ops $ sudo insmod my_cdev.ko
htritai@Rasp26:~/ldd/linux_driver_development_with_raspberrypi/07_file_cdev_fops/01_file_ops $ s
udo insmod my_cdev.ko
insmod: ERROR: could not insert module my_cdev.ko: File exists
htritai@Rasp26:~/ldd/linux_driver_development_with_raspberrypi/07_file_cdev_fops/01_file_ops $ 
sudo rmmod my_cdev 
htritai@Rasp26:~/ldd/linux_driver_development_with_raspberrypi/07_file_cdev_fops/01_file_ops $ sudo insmod my_cdev.ko
htritai@Rasp26:~/ldd/linux_driver_development_with_raspberrypi/07_file_cdev_fops/01_file_ops $ ls -l /dev/my_cdev0 
crw------- 1 root root 236, 0 Mar 29 14:02 /dev/my_cdev0
htritai@Rasp26:~/ldd/linux_driver_development_with_raspberrypi/07_file_cdev_fops/01_file_ops $ ./test1 /dev/my_cdev0 
Failed to open 0
: Permission denied
htritai@Rasp26:~/ldd/linux_driver_development_with_raspberrypi/07_file_cdev_fops/01_file_ops $ sudo ./test1 /dev/my_cdev0 
/dev/my_cdev0: file opend 0
/dev/my_cdev0: file closed 0
/dev/my_cdev0: file opend 1
/dev/my_cdev0: file closed 1
/dev/my_cdev0: file opend 2
/dev/my_cdev0: file closed 2
/dev/my_cdev0: file opend 3
/dev/my_cdev0: file closed 3
htritai@Rasp26:~/ldd/linux_driver_development_with_raspberrypi/07_file_cdev_fops/01_file_ops $ sudo rmmod my_cdev 
htritai@Rasp26:~/ldd/linux_driver_development_with_raspberrypi/07_file_cdev_fops/01_file_ops $ 
```

```bash
htritai@Rasp26:~/ldd/linux_driver_development_with_raspberrypi/07_file_cdev_fops/01_file_ops $ dmesg -W
[14079.334678] my_cdev: Caracter device registerd, Major number: 236 Minor number: 0
[14079.334698] my_cdev: Created device number under /sys/class/my_class
[14079.334703] my_cdev: Created new device node /dev/my_cdev
[14148.267207] my_cdev: my_open called, Major: 236 Minor: 0
               my_cdev: file->f_pos: 0
               my_cdev: file->f_mode: 0x1d
               my_cdev: file->f_flags: 0x20000
[14148.267330] my_cdev: my_release called, file is closed
[14148.267343] my_cdev: my_open called, Major: 236 Minor: 0
               my_cdev: file->f_pos: 0
               my_cdev: file->f_mode: 0x1e
               my_cdev: file->f_flags: 0x20001
[14148.267355] my_cdev: my_release called, file is closed
[14148.267365] my_cdev: my_open called, Major: 236 Minor: 0
               my_cdev: file->f_pos: 0
               my_cdev: file->f_mode: 0x1f
               my_cdev: file->f_flags: 0x20002
[14148.267376] my_cdev: my_release called, file is closed
[14148.267385] my_cdev: my_open called, Major: 236 Minor: 0
               my_cdev: file->f_pos: 0
               my_cdev: file->f_mode: 0x1f
               my_cdev: file->f_flags: 0x121002
[14148.267396] my_cdev: my_release called, file is closed
[14203.534991] my_cdev: Goodbye, Kernel

```

***********************





