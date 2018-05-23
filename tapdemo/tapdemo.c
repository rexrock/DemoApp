/* 
 * Author : rexrock
 * https://www.kernel.org/doc/Documentation/networking/tuntap.txt
 * https://github.com/xl0/uml-utilities/tree/master/tunctl
 */
#include <linux/if_tun.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/ioctl.h>
#include <linux/if.h>
#include <error.h>
#include <errno.h>
#include <getopt.h>

char* const short_options = "n:i:";
struct option long_options[] = {
    { "name", required_argument, NULL, 0 },
    { "interval", required_argument, NULL, 0 },
    { 0, 0, 0, 0}
};

const char *packet_str = "333300000016ae296032bb6086dd600000000024000100000000000000000000000000000000ff0200000000000000000000000000163a000502000001008f00b3f70000000104000000ff0200000000000000000001ff32bb60";
unsigned char send_buf[1024] = {0};
int tap_fd = 0, send_len = 0, interval = 600;

void HexStrToByte(const char* source, unsigned char* dest, int sourceLen) { 
    short i;  
    unsigned char highByte, lowByte;  
      
    for (i = 0; i < sourceLen; i += 2) { 
        highByte = toupper(source[i]);  
        lowByte  = toupper(source[i + 1]);  
  
        if (highByte > 0x39)  
            highByte -= 0x37;  
        else  
            highByte -= 0x30;  
  
        if (lowByte > 0x39)  
            lowByte -= 0x37;  
        else  
            lowByte -= 0x30;  
  
        dest[i / 2] = (highByte << 4) | lowByte;  
    }  
    return ;  
}

void *SendToTAP(void *arg) {
    // we write data to tap device and we can tcpdump the 
    // packet from tap interface in another shell
    while (1) {
        sleep(interval);
        // write back the packet to tap
        send_len = strlen(packet_str) + 1;
        HexStrToByte(packet_str, send_buf, strlen(packet_str));
		int num = write(tap_fd, send_buf, send_len);
        printf("send %d-%d bytes to tap interface\n", send_len, num);
	}  
}

int TapCreate(char *dev, int flags) { 
	int fd, err;
	struct ifreq ifr;

    // create network device 
	if ((fd = open("/dev/net/tun", O_RDWR|O_NONBLOCK)) < 0)
		return fd;

    // set tapif name
	memset(&ifr, 0, sizeof(ifr));
	ifr.ifr_flags |= flags;
	if (*dev != '\0')
		strncpy(ifr.ifr_name, dev, IFNAMSIZ);
	if ((err = ioctl(fd, TUNSETIFF, (void *)&ifr)) < 0) {
		close(fd);
		return err;
	}

    return fd;  
}  
  
int main(int argc, char *argv[]) { 
	char *tap_name = NULL;
	unsigned char buf[1024];
 
    // get the tap's name 
    int c;
    while((c = getopt_long(argc, argv, short_options, long_options, NULL)) != -1) {
        switch (c) {
            case 'n':
                tap_name = strdup(optarg);
                printf("tap's name is %s\n",tap_name);
                break;
            case 'i':
                interval = atoi(optarg);
                interval = interval > 0 ? interval : 600;
                printf("the time interval between write operations is %ds\n", interval);
                break;
            default:
                printf("what is wrong?\n");
                break;
        }
    }

    if (NULL == tap_name) {
        printf("Usage : %s -n tap_name [-i interval]\n", argv[0]);
        return -1;
    }

    // create tap device
	tap_fd = TapCreate(tap_name, IFF_TAP | IFF_NO_PI);  
	if (tap_fd < 0) {  
		perror("TapCreate"); 
		return 1;  
	}  

    // create thread to write
    pthread_t a_thread;
    pthread_create(&a_thread, NULL, SendToTAP, NULL);

    // handle the packets that tap receive
    fd_set fdset;
    FD_ZERO(&fdset);
    FD_SET(tap_fd , &fdset);
    while(1) {
        // wait
        int num = select(tap_fd + 1, &fdset, NULL, NULL, NULL);
        if (0 > num) {
            if (EINTR == errno || EAGAIN == errno || EWOULDBLOCK == errno) {
                continue;
            }
        }
        
        // read
        int bytes = read(tap_fd, buf, sizeof(buf));
		if (bytes < 0) { 
			continue;  
		}  
		printf("read all %d bytes from tap\n", bytes);  

        // print
        int i = 0;
		for(i = 0; i < bytes; i++) { 
			printf("%02x ",buf[i]);  
		}  
		printf("\n");  
    }

	return 0;  
} 
