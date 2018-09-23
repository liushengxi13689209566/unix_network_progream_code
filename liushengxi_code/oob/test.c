/*************************************************************************
	> File Name: test.c
	> Author: Liu Shengxi 
	> Mail: 13689209566@163.com
	> Created Time: 2018年09月23日 星期日 12时35分16秒
 ************************************************************************/

#include<stdio.h>
void fun_client00000(int connfd)
{
	char sendline[MAXLINE] = {0};
	char recvline[MAXLINE] = {0};
	int tt = 0;

	heartbeat_cli(connfd, 1, 5); // 1.调用函数

	while (Fgets(sendline, MAXLINE, stdin) != NULL)
	{
		tt = Sendlen(connfd, sendline, strlen(sendline), 0);

		if (Recvline(connfd, recvline, sizeof(recvline), 0) == 0)
		{
			printf("服 务 器 异 常 退 出\n");
			Close(connfd);
			return;
		}
		Fputs(recvline, stdout);
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
	servaddr.sin_port = htons(SERV_PORT); // 9877
	inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

	connect(sockfd, (SA *)&servaddr, sizeof(servaddr));

	fun_client00000(sockfd);

	exit(0);
}
