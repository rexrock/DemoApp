#include <sys/un.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>

#define MAX_SEND 1024
#define SERVER_PATH "/tmp/sock_demo"

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

void dump_unix(int sock_fd)
{
    char tmp[MAX_SEND] = {0};
    while(fgets(tmp, MAX_SEND, stdin) != NULL)
    {
        sendFunc(sock_fd, tmp);
        memset(tmp, 0, MAX_SEND);
    }
}

int main(int argc, char** argv)
{
    int conn_sock = socket(AF_LOCAL, SOCK_STREAM, 0);
    if(conn_sock == -1)
    {
        perror("socket fail ");
        return -1;
    }
    struct sockaddr_un addr;
    bzero(&addr, sizeof(addr));
    addr.sun_family = AF_LOCAL;
    strcpy((void*)&addr.sun_path, SERVER_PATH);
    if(connect(conn_sock, (struct sockaddr*)&addr, sizeof(addr)) < 0)
    {
        perror("connect fail ");
        return -1;
    }
    dump_unix(conn_sock);
    close(conn_sock);
    return 0;
}
