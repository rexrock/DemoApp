#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>

#include <iostream>

#define MAX_RECV 1024
#define SERVER_PATH "/tmp/sock_demo"

int recvFunc(int rfd, std::string &msg)
{
    if (0 > rfd) return -1;

    char recv_buff[MAX_RECV] = {0};
    int recv_len, data_len, temp_len;

    /* recv msg header */
    recv_len = read(rfd, recv_buff, 8);
    printf("step[1] >> recv_buff is \"%s\"\n", recv_buff);
    if (8 == recv_len && 0 == strcmp(recv_buff, "vsec:")) {
        /* recv msg length */ 
        recv_len = read(rfd, recv_buff, 10);
        printf("step[2] >> recv_buff is \"%s\"\n", recv_buff);
        if (10 == recv_len) {
            temp_len = 0;
            data_len = atoi(recv_buff);    
            if (0 >= data_len) return -2;  
            /* recv msg content */
            while(temp_len < data_len) {   
                int recv_max = data_len - temp_len;
                recv_max = MAX_RECV < recv_max ? MAX_RECV : recv_max; 
                recv_len = read(rfd, recv_buff, recv_max);
                if (0 > recv_len) break;       
                printf("step[3] date_len=%d, temp_len=%d, recv_len=%d, recv_buff is \"%s\"\n", data_len, temp_len, recv_len, recv_buff); 
                msg += recv_buff;
                temp_len += recv_len;
            }
            if (temp_len >= data_len) return data_len;
        }
    }

    printf("recv failed date_len=%d temp_len=%d recv_len=%d\n", data_len, temp_len, recv_len);
    return 0 == recv_len ? -3 : -4;
}

int sendFunc(int wfd, const char *msg)
{
    int ret = 0, save_errno, send_len;
    char head_buff[32];
    if (0 >= wfd) return -1;
    if (NULL == msg || 0 == msg[0]) return -2;

    printf("pipe_send : %s\n", msg);
    do {
        send_len = strlen(msg);

        /* send head */
        snprintf(head_buff, 32, "vsec:");
        ret = write(wfd, head_buff, 8);
        printf("step[1] >> send_buff is \"%s\" and ret is %d\n", head_buff, ret);
        if (8 != ret) {
            ret = -3;
            break;
        }

        /* send length */
        snprintf(head_buff, 32, "%010d", send_len+1);
        ret = write(wfd, head_buff, 10);
        printf("step[2] >> send_buff is \"%s\" and ret is %d\n", head_buff, ret);
        if (10 != ret) {
            ret = -4;
            break;
        }

        /* send message */
        ret = write(wfd, msg, strlen(msg)+1);
        printf("step[3] >> send_buff is \"%s\" and ret is %d\n", msg, ret);
        if (send_len+1 != ret) {
            ret = -5;
            break;
        }
    }while(0);

    printf("pipe_send : ret=%d\n", ret);
    return ret;
}

int main(int argc, char** argv) {
    int server_sockfd, client_sockfd;
    socklen_t server_len, client_len;
    struct sockaddr_un server_address;
    struct sockaddr_un client_address;
    fd_set readfds, testfds;
    int result;


    // create socket
    server_sockfd = socket(AF_LOCAL, SOCK_STREAM, 0);
    if ( -1 == server_sockfd) {
        perror("socket func fail ");
        return -1;
    }

    // init sockaddr
    server_address.sun_family = AF_LOCAL;
    strncpy(server_address.sun_path, SERVER_PATH, sizeof(server_address.sun_path)-1);


    // bind sockaddr
    unlink(SERVER_PATH);
    server_len = sizeof(server_address);
    bind(server_sockfd, (struct sockaddr*)&server_address, server_len);

    // listen
    listen(server_sockfd, 5);
    FD_ZERO(&readfds);
    FD_SET(server_sockfd, &readfds);

    while(1) {
        testfds = readfds;
        printf("server waitting......\n");
        result = select(FD_SETSIZE, &testfds, NULL, NULL, NULL);

        if (result < 1) perror("select error : ");

        for (int fd = 0; fd < FD_SETSIZE; fd++)
        {
            if (!FD_ISSET(fd, &testfds)) continue;

            if (fd == server_sockfd)
            {
                client_len = sizeof(struct sockaddr_un);
                client_sockfd = accept(server_sockfd, (struct sockaddr *)&client_address, &client_len);
                FD_SET(client_sockfd, &readfds);
                printf("adding client %d\n", client_sockfd);
            }else
            {
                std::string msg;
                recvFunc(fd, msg);
                printf("==%s\n", msg.c_str());
            }
        }
        sleep(6);
    }
    close(server_sockfd);
    return 0;
}

