#include "../myhead.h"
void fun(int connfd)
{
	ssize_t n;
	TEST test;
	while (1)
	{
		bzero(&test, sizeof(test));
		n = Recvlen(connfd, &test, sizeof(TEST), 0);
		printf("n== %d\n", n);
		if (n <= 0)
		{
			printf("对端关闭\n");
			Close(connfd);
			break;
		}
		Sendlen(connfd, &test, sizeof(TEST), 0);
	}
}
int main(int argc, char **argv)
{
	int listenfd, connfd;
	pid_t childpid;
	socklen_t clilen;
	struct sockaddr_in cliaddr, servaddr;

	listenfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT); //9877

	int opt = 1;
	setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (int *)&opt, sizeof(int));

	Bind(listenfd, (SA *)&servaddr, sizeof(servaddr));

	Listen(listenfd, LISTENQ);

	for (;;)
	{
		clilen = sizeof(cliaddr);
		connfd = Accept(listenfd, (SA *)&cliaddr, &clilen);
		if ((childpid = Fork()) == 0)
		{					 /* child process */
			Close(listenfd); /* close listening socket */

			printf("新的连接：connfd== %d \n", connfd);

			fun(connfd); /* process the request */

			exit(0);
		}
		Close(connfd); /* parent closes connected socket */
	}
}
