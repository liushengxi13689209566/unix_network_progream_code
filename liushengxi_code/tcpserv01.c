#include "origin.h"

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

	Bind(listenfd, (SA *)&servaddr, sizeof(servaddr));

	Listen(listenfd, LISTENQ);

	for (;;)
	{
		clilen = sizeof(cliaddr);
		printf("111\n");
		connfd = Accept(listenfd, (SA *)&cliaddr, &clilen, 3); //设置三秒时间，用于简单测试
		printf("222\n");

		sleep(10);
		close(connfd);
	}
	close(listenfd);
}
