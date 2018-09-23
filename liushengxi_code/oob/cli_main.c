/*************************************************************************
	> File Name: heartbeatcli.c
	> Author: Liu Shengxi 
	> Mail: 13689209566@163.com
	> Created Time: 2018年09月16日 星期日 22时42分26秒
 ************************************************************************/
/*这是客户端哦！！！！ */

#include "../myhead.h"
#include "test.h"

void fun_client(FILE *fp, int sockfd)
{
	int maxfdp1, stdineof = 0;
	fd_set rset;
	char recvline[MAXLINE], sendline[MAXLINE];
	int n;
	
	heartbeat_cli(sockfd, 1, 5); // 1.调用函数

	FD_ZERO(&rset);
	for (;;)
	{
		if (stdineof == 0)
			FD_SET(fileno(fp), &rset);
		FD_SET(sockfd, &rset);
		maxfdp1 = max(fileno(fp), sockfd) + 1;

		if ((n = select(maxfdp1, &rset, NULL, NULL, NULL)) < 0)
		{
			if (errno == EINTR) // 2.处理 select
				continue;
			else
				err_sys("select error");
		}

		if (FD_ISSET(sockfd, &rset))
		{
			if ((n = Recvline(sockfd, recvline, MAXLINE, 0)) == 0) //读取一行,Readline 返回读取到的字节数
			{
				if (stdineof == 1)
					return;
				else
					err_quit("fun_client:server terminated permaturely ");
			}

			write(STDOUT_FILENO, recvline, n); //3.调用 write 函数，而不是 fputs
		}
		if (FD_ISSET(fileno(fp), &rset)) //可以进行输入了
		{
			if (Fgets(sendline, MAXLINE, fp) == NULL)
			{
				stdineof = 1;
				shutdown(sockfd, SHUT_WR);
				FD_CLR(fileno(fp), &rset);
				continue;
			}

			Sendlen(sockfd, sendline, strlen(sendline), 0); 
		}
	}
}
/*void fun_client00000(int connfd)
{
	char sendline[MAXLINE] = {0};
	char recvline[MAXLINE] = {0};
	int tt = 0;

	while (Fgets(sendline, MAXLINE, stdin) != NULL)
	{
		tt = Sendlen(connfd, sendline, strlen(sendline), 0);

		if ( Recvline(connfd, recvline, sizeof(recvline), 0) == 0)
		{
			printf("服 务 器 关 闭 连 接 退 出 \n");
			Close(connfd);
			return;
		}
		Fputs(recvline, stdout);
	}
}*/
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
	servaddr.sin_port = htons(SERV_PORT); // 9877
	inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

	connect(sockfd, (SA *)&servaddr, sizeof(servaddr));

	fun_client(stdin,sockfd);

	exit(0);
}