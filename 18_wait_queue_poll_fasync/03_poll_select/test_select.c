// test_select.c
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/select.h>

int main(int argc, char *argv[])
{
    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        perror("open failed");
        return 1;
    }

    char buffer[64];

    printf("Waiting for button press... (Ctrl+C to exit)\n");

    while (1) {

        fd_set read_fds;
        FD_ZERO(&read_fds);
        FD_SET(fd, &read_fds);  /* Must rebuild the set before every call */

        /* Block until fd becomes readable (NULL timeout = wait forever) */
        int ret = select(fd + 1, &read_fds, NULL, NULL, NULL);

        if (ret < 0) {
            perror("select failed");
            break;
        }

        if (FD_ISSET(fd, &read_fds)) {

            int n = read(fd, buffer, sizeof(buffer) - 1);
            if (n < 0) {
                perror("read failed");
                break;
            }

            buffer[n] = '\0';
            printf("Received: %s \n", buffer);
        }
    }

    close(fd);
    return 0;
}