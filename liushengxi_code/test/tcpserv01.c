#include "../myhead.h"
struct epoll_event events[1024];
int epfd;
void fun(int *arg)
{
	ssize_t n;
	struct epoll_event event;
	int connfd = *(int *)arg;
	TEST test;
	bzero(&test, sizeof(test));
	n = Recvlen(connfd, &test, sizeof(TEST), 0);
	printf("n== %ld\n", n);
	if (n == 0)
	{
		printf("对端关闭\n");
		epoll_ctl(epfd, EPOLL_CTL_DEL, connfd, &event);
		Close(connfd);
		pthread_exit(0);
	}
	else if (n > 0)
		Sendlen(connfd, &test, sizeof(TEST), 0);
	else
		printf("Other error !!!!!\n");
}
int main(int argc, char **argv)
{
	int listenfd;
	socklen_t clilen;
	struct sockaddr_in servaddr;

	listenfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT); //9877

	int opt = 1;
	setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (int *)&opt, sizeof(int));

	Bind(listenfd, (SA *)&servaddr, sizeof(servaddr));

	Listen(listenfd, LISTENQ);

	struct epoll_event ev;
	epfd = epoll_create(256);
	ev.data.fd = listenfd;
	ev.events = EPOLLIN;
	epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &ev); //加入
	signal(SIGPIPE, SIG_IGN);
	int conn_fd = 0;
	pthread_t tid;
	int nfds = 0, i;
	struct sockaddr_in clientaddr; //客户机地址
	clilen = sizeof(struct sockaddr_in);
	for (;;)
	{
		nfds = epoll_wait(epfd, events, 1024, 500);
		for (i = 0; i < nfds; i++)
		{
			int sockfd = events[i].data.fd;
			if (sockfd == listenfd) //如果新监测到一个SOCKET用户连接到了绑定的SOCKET端口 ， 建立新的连接
			{
				conn_fd = accept(listenfd, (struct sockaddr *)&clientaddr, &clilen);
				if (conn_fd < 0)
					printf("server accept error");

				printf("\t\t accept a connection from %s\n", inet_ntoa(clientaddr.sin_addr));
				ev.data.fd = conn_fd;
				ev.events = EPOLLIN;
				epoll_ctl(epfd, EPOLL_CTL_ADD, conn_fd, &ev); //epfd------epoll 句柄
			}
			else if (events[i].events & EPOLLIN) //如果是已经连接的用户，并且收到数据，那么进行读入
			{
				if (pthread_create(&tid, NULL, (void *)fun, (void *)&sockfd) < 0)
					printf("server pthread_create ");
			}
		}
	}
	return 0;
}
