#include "../myhead.h"
void fun(int connfd)
{
	ssize_t n;
	char buf[1024] = {0};
	while (1)
	{
		bzero(buf, sizeof(buf));
		n = Recvline(connfd, buf, 1024, 0);

		if (n <= 0)
		{
			printf("对端关闭\n");
			Close(connfd);
			break;
		}
		Sendlen(connfd, buf, n, 0);
	}
}
int main(int argc, char **argv)
{
	int listenfd, connfd;
	pid_t childpid;
	socklen_t clilen;
	struct sockaddr_in cliaddr, servaddr;

	const char *ip = argv[1];
	const int port = atoi(argv[2]);

	listenfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	//servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	inet_pton(AF_INET, ip, &servaddr.sin_addr);
	servaddr.sin_port = htons(port); //9877

	int opt = 1;
	setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (int *)&opt, sizeof(int));
	setsockopt(listenfd, SOL_SOCKET, SO_REUSEPORT, (int *)&opt, sizeof(int));

	Bind(listenfd, (SA *)&servaddr, sizeof(servaddr));

	Listen(listenfd, LISTENQ);

	for (;;)
	{
		clilen = sizeof(cliaddr);
        printf("stsrt sleep !!!!!!\n");
		//sleep(100);
        printf("end sleep ......\n");
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
