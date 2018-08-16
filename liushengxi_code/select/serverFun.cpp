/***	**********************************************************************
	> File Name: serverFun.cpp
	> Author: Liu Shengxi 
	> Mail: 13689209566@163.com
	> Created Time: 2018年08月14日 星期二 15时05分32秒
 ************************************************************************/

#include "myhead.h"
#include "origin.h"
int mySelect::run(const char *ip, const int port)
{
	struct sockaddr_in servaddr;

	listenfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	inet_pton(AF_INET, ip, &servaddr.sin_addr);
	servaddr.sin_port = htons(port);

	Bind(listenfd, (SA *)&servaddr, sizeof(servaddr));

	Listen(listenfd, LISTENQ);

	maxfd = listenfd; //0,1,2 被标准I/O占用，所以第一个可用描述符是 3 ，先初始化
	maxi = -1;

	for (int i = 0; i < FD_SETSIZE; ++i)
		client[i] = -1;
	FD_ZERO(&allset);
	FD_SET(listenfd, &allset); //打开监听套接字对应位

	for (;;)
	{
		rset = allset;
		int nready = Select(maxfd + 1, &rset, NULL, NULL, NULL);
		if (FD_ISSET(listenfd, &rset))
		{
			struct sockaddr_in cliaddr;
			socklen_t clilen = sizeof(cliaddr);
			int connfd = Accept(listenfd, (SA *)&cliaddr, &clilen);
			/*
		printf("new client: %s, port %d\n",
					Inet_ntop(AF_INET, &cliaddr.sin_addr, 4, NULL),
					ntohs(cliaddr.sin_port));
		*/
			int i;
			for (i = 0; i < FD_SETSIZE; i++)
			{
				if (client[i] < 0)
				{
					client[i] = connfd;
					break;
				}
			}
			if (i == FD_SETSIZE)
				err_sys("too many clients ");
			FD_SET(connfd, &allset); //加入新的套接字
			if (connfd > maxfd)
				maxfd = connfd; //为了select 函数的第一个参数
			if (i > maxi)
				maxi = i;
			if (--nready <= 0)
				continue;
		}
		int sockfd;
		ssize_t n;
		char buf[MAXLINE];
		for (int i = 0; i <= maxi; i++)
		{
			if ((sockfd = client[i]) < 0)
				continue;
			if (FD_ISSET(sockfd, &rset))
			{ //看描述符是否在select 返回的描述符集中
				bzero(&buf, sizeof(buf));
				if ((n = Read(sockfd, buf, MAXLINE)) == 0)
				{ //对方关闭连接
					Close(sockfd);
					FD_CLR(sockfd, &allset);
					client[i] = -1;
				}
				else
					Writen(sockfd, buf, n);
				if (--nready <= 0)
					break;
			}
		}
	}
}
mySelect::~mySelect()
{
	close(listenfd);
}
