#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h> // for ioctl system call _IO, _IOR, _IOW macros and ioctl function

#define DEV_BUFFER_SIZE 64

// copy from my_cdev.c, they must be the same as the ones defined in the kernel module, otherwise ioctl commands won't work
#define MYCDEV_MAGIC            'M'                         // Magic number for ioctl commands
#define MYCDEV_CLEAR            _IO(MYCDEV_MAGIC, 1)        // Command to clear the device's internal buffer, no data transfer
#define MYCDEV_SAY_HELLO        _IO(MYCDEV_MAGIC, 2)        // Command to make the kernel print a hello message, no data transfer
#define MYCDEV_USER_READ        _IOR(MYCDEV_MAGIC, 3, int)  // ** IOR: User read - Kernrl write + type, command to read an integer value from the kernel, kernel writes data to user-space
#define MYCDEV_USER_WRITE       _IOW(MYCDEV_MAGIC, 4, int)  // ** IOW: User write - Kernrl read + type, command to write an integer value to the kernel, user-space writes data to kernel

int main(int argc, char *argv[])
{
    if (argc < 0)
    {
        printf("I need the file to open an argument!\n");
        return 0;
    }
    
    int fd;
    int ret;
    /* ***************************************** */
    // open()
    fd = open(argv[1], O_RDWR);
    if (fd < 0)
    {
        perror("Failed to open 0\n");
        return fd;
    }
    printf("%s: file opend 0\n", argv[1]);

    // write()
    ssize_t len = write(fd, argv[2], strlen(argv[2]));
    if (len < 0)
    {
        perror("Failed to write\n");
        goto close_fd;
    }
    printf("Written length: %ld \n", len);
    printf("Press enter to continue.");
    getchar();

    // ioctl()  MYCDEV_SAY_HELLO
    if (ioctl(fd, MYCDEV_SAY_HELLO) < 0) // -1 on failure.
    {
        perror("Failed ioctl MYCDEV_SAY_HELLO\n");
        goto close_fd;
    }
    printf("Kernel said hello\n");

    // ioctl()  MYCDEV_USER_READ
    int value;
    if (ioctl(fd, MYCDEV_USER_READ, &value) < 0)
    {
        perror("Failed ioctl MYCDEV_USER_READ\n");
        goto close_fd;
    }
    printf("Value: 0x%x copied from kernel\n", value);

    // ioctl()  MYCDEV_USER_WRITE
    value = 0xb00b00;
    if (ioctl(fd, MYCDEV_USER_WRITE, &value) < 0)
    {
        perror("Failed ioctl MYCDEV_USER_WRITE\n");
        goto close_fd;
    }
    printf("Value: 0x%x copied to kernel\n", value);

    // read()
    char buffer[DEV_BUFFER_SIZE] = {0};
    len = read(fd, buffer, DEV_BUFFER_SIZE);
    if (len < 0)
    {
        perror("failed to read\n");
        goto close_fd;
    }
    printf("read: %d bytes - %s \n", len, buffer);
  
    // ioctl()  MYCDEV_CLEAR
    if (ioctl(fd, MYCDEV_CLEAR) < 0)
    {
        perror("Failed ioctl MYCDEV_CLEAR\n");
        goto close_fd;
    }
    printf("Kernel private buffer cleared\n");
    memset(buffer, 0, DEV_BUFFER_SIZE);

    // read()
    len = read(fd, buffer, DEV_BUFFER_SIZE);
    if (len < 0)
    {
        perror("failed to read\n");
        goto close_fd;
    }
    printf("read: %d bytes - %s \n", len, buffer);

close_fd:
    ret = close(fd);
    if (ret < 0)
    {
        perror("Failed to close 0\n");
        return ret;
    }
    printf("%s: file closed 0\n", argv[1]);

    return 0;
}


/*
Terminal 1:
$ sudo insmod my_cdev.ko
$ ls -l /dev/my_cdev0 
crw------- 1 root root 236, 0 Mar 29 19:01 /dev/my_cdev0


$ sudo ./a.out /dev/my_cdev0 "hello world"
/dev/my_cdev0: file opend 0
Written length: 11 
Press enter to continue.

Kernel said hello

Value: 0xc0ffee copied from kernel

Value: 0xb00b00 copied to kernel

read: 11 bytes - hello world 

Kernel private buffer cleared

read: 0 bytes -  

/dev/my_cdev0: file closed 0

$ sudo rmmod my_cdev 

Terminal 2:
$ dmesg -W
[32030.522072] my_cdev: Caracter device registerd, Major number: 236 Minor number: 0
               my_cdev: Created device number under /sys/class/my_class
               my_cdev: Created new device node /dev/my_cdev


[32070.459603] my_cdev: my_open called, Major: 236 Minor: 0
[32070.459638] my_cdev: Allocated 64 bytes for private data
[32070.459929] my_cdev: write request=11, will copy=11
[32070.459946] my_cdev: write done: copied=11


[32079.598781] my_cdev: Hello from kernel via ioctl

[32079.598827] my_cdev: value: 0xc0ffee copied to user

[32079.598844] my_cdev: value: 0xb00b00 copied from user

[32079.598854] my_cdev: read called: request=64, 
[32079.598860] my_cdev: Read will copy=11 bytes
[32079.598863] my_cdev: Read done: return=11

[32079.598874] my_cdev: private data field cleared

[32079.598882] my_cdev: read called: request=64, 
[32079.598885] my_cdev: Read will copy=0 bytes
[32079.598888] my_cdev: Read done: return=0

[32079.598899] my_cdev: my_release called, freed 64 bytes and file is closed

[32149.158251] my_cdev: Goodbye, Kernel
*/