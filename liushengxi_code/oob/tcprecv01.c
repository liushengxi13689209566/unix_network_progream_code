/*************************************************************************
	> File Name: tcprecv01.c
	> Author: Liu Shengxi 
	> Mail: 13689209566@163.com
	> Created Time: 2018年09月13日 星期四 20时56分29秒
 ************************************************************************/

#include "../myhead.h"

int listenfd, connfd;
void sig_urg(int signo)
{
	int n;
	char buff[100];

	printf("SIGURG recvied\n");
	n = recv(connfd, buff, sizeof(buff) - 1, MSG_OOB);
	buff[n] = 0;
	printf("recv %d OOb bytes: %s \n", n, buff);
}
int main(int argc, char **argv)
{
	int n;
	char buff[100];
	if (argc == 2)
		listenfd = Tcp_listen(NULL,argv[1],NULL);
	else if (argc == 3)
		listenfd = Tcp_listen(argv[1],argv[2],NULL);
	else
		err_quit("usage : tcprecv01 <host> <port>");
	connfd = Accept(listenfd, NULL, NULL);
	signal(SIGURG, sig_urg);
	Fcntl(connfd, F_SETOWN, getpid());

	for (;;)
	{
		if ((n = recv(connfd, buff, sizeof(buff) - 1,0)) == 0)
		{
			printf("recvied EOF\n");
			exit(0);
		}
		buff[n] = 0;
		printf("read %d bytes :%s \n", n, buff);
	}
}

