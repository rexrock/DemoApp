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
    int server_sockfd, client_sockfd;
    int server_len, client_len;
    struct sockaddr_in server_address;
    struct sockaddr_in client_address;
    fd_set readfds, testfds;
    int result;

    server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(9876);
    server_len = sizeof(server_address);
    bind(server_sockfd, (const struct sockaddr *)&server_address, server_len);

    listen(server_sockfd, 5);

    FD_ZERO(&readfds);
    FD_SET(server_sockfd, &readfds);

    while (1) 
    {
        testfds = readfds;
        printf("server waitting......\n");
        result = select(FD_SETSIZE, &testfds, NULL, NULL, NULL);
        if (result < 1) perror("select error : ");

        for (int fd = 0; fd < FD_SETSIZE; fd++)
        {
            if (!FD_ISSET(fd, &testfds)) continue;

            if (fd == server_sockfd)
            {
                client_len = sizeof(struct sockaddr_in);
                client_sockfd = accept(server_sockfd, (struct sockaddr *)&client_address, &client_len);
                FD_SET(client_sockfd, &readfds);
                printf("adding client on fd[%d] with ip=%x port=%d\n", client_sockfd, client_address.sin_addr.s_addr, ntohs(client_address.sin_port));
            }else
            {
                char ch = '?';
                read(fd, &ch, 1);
                printf("client[fd=%d ip=%x port=%d] : %c\n", client_sockfd, client_address.sin_addr.s_addr, ntohs(client_address.sin_port), ch);
            }
        }
        sleep(6);
    }
}

int main(int argc, char *argv[])
{
    do_run();
    return 0;
}
