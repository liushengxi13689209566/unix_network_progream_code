#include "../myhead.h"
#include "test.h"

void fun_client(int connfd)
{
	char sendline[MAXLINE] = {0};
	char recvline[MAXLINE] = {0};
	int tt = 0, n;

	heartbeat_cli(connfd, 1, 5);

	while (Fgets(sendline, MAXLINE, stdin) != NULL)
	{
		tt = Sendlen(connfd, sendline, strlen(sendline), 0);
		/*阻塞于 Read 函数 */
		if (recv(connfd, recvline, sizeof(recvline), 0) == 0)
		{
			printf(" 对端关闭,客户端退出 \n");
			Close(connfd);
			exit(0);
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
	servaddr.sin_port = htons(SERV_PORT);
	inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

	connect(sockfd, (SA *)&servaddr, sizeof(servaddr));

	fun_client(sockfd);

	exit(0);
}
