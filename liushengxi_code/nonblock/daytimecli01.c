/*************************************************************************
	> File Name: daytimecli01.c
	> Author: Liu Shengxi 
	> Mail: 13689209566@163.com
	> Created Time: 2018年08月22日 星期三 21时57分28秒
 ************************************************************************/

#include "../myhead.h"
int Connect_nonblock(int sockfd, const SA *saptr, socklen_t salen, int nsec) //返回 -1 失败
{
	int flags, n, error;
	socklen_t len;
	fd_set rset, wset;
	struct timeval tval;
	flags = Fcntl(sockfd, F_GETFL, 0);
	Fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);

	error = 0;
	if ((n = connect(sockfd, saptr, salen)) < 0)
	{
		if (errno != EINPROGRESS)
			return (-1);
	}
	if (n == 0)
		goto done;

	FD_ZERO(&rset);
	FD_SET(sockfd, &rset);
	wset = rset;
	tval.tv_sec = nsec;
	tval.tv_usec = 0;
	if ((n = Select(sockfd + 1, &rset, &wset, NULL, nsec ? &tval : NULL)) == 0) //返回0，超时
	{
		Close(sockfd);
		errno = ETIMEDOUT;
		return (-1);
	}
	if (FD_ISSET(sockfd, &rset) || FD_ISSET(sockfd, &wset))
	{
		len = sizeof(error);
		if (getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, &len) < 0)
			return (-1);
	}
	else
		err_quit("selcet error :sockfd not set\n");
done: //直到建立才返回

	Fcntl(sockfd, F_SETFL, flags);

	if (error)
	{
		Close(sockfd);
		errno = error;
		return (-1);
	}
	return 0; //成功连接
}

int main(int argc, char **argv)
{
	int sockfd, n;
	char recvline[MAXLINE + 1] = {0};
	struct sockaddr_in servaddr;

	if (argc != 2)
		err_quit("usage: a.out <IPaddress>");

	if ((sockfd = Socket(AF_INET, SOCK_STREAM, 0)) < 0)
		err_sys("Socket error");

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(13); /* daytime server */
	if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0)
		err_quit("inet_pton error for %s", argv[1]);

	if (Connect_nonblock(sockfd, (SA *)&servaddr, sizeof(servaddr), 10) < 0)
		err_sys("connect error");

	while ((n = recv(sockfd, recvline, MAXLINE, 0)) > 0)
	{
		recvline[n] = 0; /* null terminate */
		printf("recvline ==  %s\n", recvline);
	}
	if (n < 0)
		err_sys("read error");
	return 0;
}
