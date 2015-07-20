#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#define BUFSIZE 1024

int main()
{
    int fd = open("./datafile", O_CREAT | O_APPEND);
    if (-1 == fd) {
        fprintf(stderr, "Error[%d], ", errno);
        perror(NULL);
    }

    char buf[BUFSIZE] = {'\0'};
    int count = read(fd, buf, BUFSIZE);
    if (count) {
        fprintf(stdout, "%s", buf);
    }

    close(fd);
    return fd;
}
