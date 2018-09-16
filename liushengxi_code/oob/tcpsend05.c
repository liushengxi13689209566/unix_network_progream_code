/*************************************************************************
	> File Name: tcpSendlen01.c
	> Author: Liu Shengxi 
	> Mail: 13689209566@163.com
	> Created Time: 2018年09月13日 星期四 20时56分15秒
 ************************************************************************/

#include "../myhead.h"
int main(int argc, char **argv)
{
	int sockfd;
	if (argc != 3)
		err_quit("usage：tcpSendlen01 <host> <port#> ");
	sockfd = Tcp_connect(argv[1], argv[2]);

	int size = 32768;
	Setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, &size, sizeof(size)); //设置发送缓冲区大小

	char buff[16384] = {0};
	Sendlen(sockfd, buff, 16384, 0);
	printf("Sendlen 16384  of normal data \n");
	sleep(5);

	Sendlen(sockfd, "4", 1, MSG_OOB);
	printf("Sendlen 1 bytes  of OOB data \n");

	Sendlen(sockfd, buff, 1024, 0);
	printf("Sendlen 1024   of normal data \n");

	exit(0);
}
