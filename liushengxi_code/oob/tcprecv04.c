/*************************************************************************
	> File Name: tcprecv01.c
	> Author: Liu Shengxi 
	> Mail: 13689209566@163.com
	> Created Time: 2018年09月13日 星期四 20时56分29秒
 ************************************************************************/

#include "../myhead.h"

int main(int argc, char **argv)
{
	int n;
	int listenfd, connfd;
	char buff[100];
	if (argc == 2)
		listenfd = Tcp_listen(NULL, argv[1], NULL);
	else if (argc == 3)
		listenfd = Tcp_listen(argv[1], argv[2], NULL);
	else
		err_quit("usage : tcprecv01 <host> <port>");

	int on = 1;
	Setsockopt(listenfd, SOL_SOCKET, SO_OOBINLINE, &on, sizeof(on)); //在线接受

	connfd = Accept(listenfd, NULL, NULL);
	sleep(5);

	for (;;)
	{
		if(Sockatmark(connfd))
			printf("at OOB data !!!!!!!!!!\n");

		if ((n = recv(connfd, buff, sizeof(buff) - 1, 0)) == 0)
		{
			printf("recvied EOF\n");
			exit(0);
		}
		buff[n] = 0;
		printf("recv %d bytes :%s \n", n, buff);
	}
}
