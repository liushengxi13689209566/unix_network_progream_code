/*************************************************************************
	> File Name: heartbeatcli.c
	> Author: Liu Shengxi 
	> Mail: 13689209566@163.com
	> Created Time: 2018年09月16日 星期日 22时42分26秒
 ************************************************************************/
/*这是客户端哦！！！！ */

#include "../myhead.h"

void fun_client(FILE *fp, int connfd)
{
	int maxfdp1, stdineof;
	fd_set rset;
	char buf[MAXLINE];
	int n;
	FD_ZERO(&rset);
	stdineof = 0;
	heartbeat_cli(sockfd,1,5);
	for (;;)
	{
		if (stdineof == 0)
			FD_SET(fileno(fd), &rset);
		FD_SET(sockfd, &rset);
		maxfdp1 = max(fileno(fp), sockfd) + 1;
		Select(maxfdp1, &rset, NULL, NULL, NULL);
		if (FD_ISSET(sockfd, &rset))
		{
			if ((n = recv(sockfd, buf, MAXLINE, 0)) == 0)
			{
				if (stdineof == 1)
					return;
				else
					err_quit("fun_client:server terminated permaturely ");
			}
			send(fileno(stdout), buf, n, 0);
		}
		if (FD_ISSET(fileno(fp), &rset))
		{
			if ((n = Read(fileno(fp), buf, MAXLINE)) == 0)
			{
				stdineof = 1;
				shutdown(sockfd, SHUT_WR);
				FD_CTL(fileno(fp), &rset);
				continue;
			}
			Sendlen(sockfd, buf, n, 0);
		}
	}
}
int main(int argc, char **argv)
{
	int sockfd;
	struct sockaddr_in servaddr;

	if (argc != 2)
	{
		printf("usage: tcpcli <IPaddress>\n");
		return -1;
	}

	sockfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT);
	inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

	connect(sockfd, (SA *)&servaddr, sizeof(servaddr));

	fun_client(stdin, sockfd);

	exit(0);
}
