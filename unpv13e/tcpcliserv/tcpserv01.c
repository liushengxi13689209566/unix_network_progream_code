#include "unp.h"

int my_accept(int fd, struct sockaddr *sa, socklen_t *salenptr)
{
	int n;

again:
	if ((n = accept(fd, sa, salenptr)) < 0)
	{
#ifdef EPROTO
		if (errno == EPROTO || errno == ECONNABORTED)
#else
		if (errno == ECONNABORTED)
#endif
			goto again;
		else
			printf("accept error");
	}
	return (n);
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
	servaddr.sin_port = htons(9888);

	Bind(listenfd, (SA *)&servaddr, sizeof(servaddr));

	Listen(listenfd, LISTENQ);

	for (;;)
	{
		clilen = sizeof(cliaddr);
		connfd = my_accept(listenfd, (SA *)&cliaddr, &clilen);

		if ((childpid = Fork()) == 0)
		{					  /* child process */
			Close(listenfd);  /* close listening socket */
			str_echo(connfd); /* process the request */
			exit(0);
		}
		Close(connfd); /* parent closes connected socket */
	}
}
