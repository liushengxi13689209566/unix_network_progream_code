/*************************************************************************
	> File Name: tcprecv02.c
	> Author: Liu Shengxi 
	> Mail: 13689209566@163.com
	> Created Time: 2018年09月13日 星期四 22时36分35秒
 ************************************************************************/

#include "../myhead.h"

int main(int argc, char **argv)
{
	int n;
	char buff[100];
	int listenfd, connfd;
	fd_set rset, xset;

	if (argc == 2)
		listenfd = Tcp_listen(NULL, argv[1], NULL);
	else if (argc == 3)
		listenfd = Tcp_listen(argv[1], argv[2], NULL);
	else
		err_quit("usage : tcprecv01 <host> <port>");
	connfd = Accept(listenfd, NULL, NULL);
	FD_ZERO(&rset);
	FD_ZERO(&xset);
	for (;;)
	{
		FD_SET(connfd, &rset);
		FD_SET(connfd, &xset);

		Select(connfd + 1, &rset, NULL, &xset, NULL);

		if (FD_ISSET(connfd, &xset))
		{
			n = recv(connfd, buff, sizeof(buff) - 1, MSG_OOB);
			buff[n] = 0;
			printf("recv %d of OOB bytes : %s \n", n, buff);
		}
		if (FD_ISSET(connfd, &rset))
		{
			n = recv(connfd, buff, sizeof(buff) - 1, 0);
			if (n == 0)
			{
				printf("recvived EOF !\n");
				exit(0);
			}
			buff[n] = 0;
			printf("recv %d bytes : %s \n", n, buff);
		}
	}
}
