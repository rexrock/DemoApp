#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/ioctl.h>

#include <netinet/in.h>

int do_run()
{
    int sockfd;
    int len;
    struct sockaddr_in address;
    int result;
    char ch = 'A';

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr("127.0.0.1");
    address.sin_port = htons(9876);
    len = sizeof(address);
    result = connect(sockfd, (const struct sockaddr *)&address, len);
    if (-1 == result) perror(NULL);

    while (1)
    {
        printf("send [%c]\n", ch);
        write(sockfd, &ch, 1);
        sleep(6);
    }

    return 0;
}

int main(int argc, char *argv[])
{
    do_run();
    return 0;
}
