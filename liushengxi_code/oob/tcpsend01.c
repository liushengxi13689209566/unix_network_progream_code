/*************************************************************************
	> File Name: tcpSendlen01.c
	> Author: Liu Shengxi 
	> Mail: 13689209566@163.com
	> Created Time: 2018年09月13日 星期四 20时56分15秒
 ************************************************************************/

#include "../myhead.h"
int main(int argc,char **argv)
{
	int sockfd;
	if (argc != 3)
		err_quit("usage：tcpSendlen01 <host> <port#> ");
	sockfd = Tcp_connect(argv[1], argv[2]);

	Sendlen(sockfd, "123", 3, 0);
	printf("Sendlen 123  of normal data \n");
	sleep(1);

	Sendlen(sockfd, "4", 1, MSG_OOB);
	printf("Sendlen 4  of OOB data \n");
	sleep(1);

	Sendlen(sockfd, "56", 2, 0);
	printf("Sendlen 56   of normal data \n");
	sleep(1);

	Sendlen(sockfd, "710", 3, MSG_OOB);
	printf("Sendlen 7 of OOB data \n");
	sleep(1);

	Sendlen(sockfd, "89", 2, 0);
	printf("Sendlen 89   of normal data \n");
	sleep(1);

	exit(0);
}
