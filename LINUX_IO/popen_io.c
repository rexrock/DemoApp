#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#define BUFSIZE 1024
#define CMDLINE "bash test.sh"
//#define CMDLINE "python test.py"
//#define CMDLINE "python /opt/nsfocus/bin/snmpd/system_get_info.py -c"


int main()
{
    FILE * fp = popen(CMDLINE, "r");
    int fd = fileno(fp);

    int opts = fcntl(fd, F_GETFL, 0);
    if (opts < 0)
        return 0;
    opts &= ~O_NONBLOCK;
    opts |= O_NONBLOCK;
    if(fcntl(fd, F_SETFL, opts) < 0)
        return 0;

    char buf[BUFSIZE] = {'\0'};
    struct timeval tv = {0};
    tv.tv_usec = 1 * 1000 * 1000;
    fd_set fdset;
    FD_ZERO(&fdset);
    FD_SET(fd , &fdset);
    while(1) {
        int num = select(fd + 1, &fdset, NULL, NULL, NULL);
        if (0 > num) {
            if (EINTR == errno || EAGAIN == errno || EWOULDBLOCK == errno) {
                continue;
            }
        }
        int nbytes = read(fd, buf, BUFSIZE);
        fprintf(stderr, "%d:%s\n", nbytes, buf);
        //break;
    }

    pclose(fp);
    return fd;
}
