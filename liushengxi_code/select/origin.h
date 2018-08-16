/*************************************************************************
	> File Name: origin.h
	> Author: Liu Shengxi
	> Mail: 13689209566@163.com
	> Created Time: 2018年08月14日 星期二 15时17分38秒
 ************************************************************************/

#ifndef _ORIGIN_H
#define _ORIGIN_H
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>
#include <sys/select.h>
#include <strings.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#define MAXLINE 1024
#define SA struct sockaddr

int err_sys(const char *err_string)
{
	perror(err_string); // err_string + 出错信息
	exit(1);
}
int Socket(int family, int type, int protocol)
{
	int n;
	if ((n = socket(family, type, protocol)) < 0)
		err_sys("socket error ");
	return n;
}
int Bind(int fd, const struct sockaddr *sa, socklen_t salen)
{
	if (bind(fd, sa, salen) < 0)
		err_sys("bind error");
}
void Listen(int fd, int backlog)
{
	char *ptr = nullptr;
	if ((ptr = getenv("LISTENQ")) != NULL)
		backlog = atoi(ptr);

	if (listen(fd, backlog) < 0)
		err_sys("listen error");
}
int Select(int nfds, fd_set *readfds, fd_set *writefds,
		   fd_set *exceptfds, struct timeval *timeout)
{
	int n;
	if ((n = select(nfds, readfds, writefds, exceptfds, timeout)) < 0)
		err_sys("select error ");
	return n;
}
int Accept(int fd, struct sockaddr *sa, socklen_t *salenptr)
{
	int n;
again:
	Signal(SIGALRM,sig_alrm);
	if ((n = accept(fd, sa, salenptr)) < 0)
	{
		/*ECONNABORTED:在于当服务和客户进程在完成用于 TCP 连接的“三次握手”后，客户 TCP 却发送了一个 RST （复位）分节，
		在服务进程看来，就在该连接已由 TCP 排队，等着服务进程调用 accept 的时候 RST 却到达了。
		POSIX 规定此时的 errno 值必须 ECONNABORTED */
		/*EPROTO 与 ECONNABORTED 是一样的，看具体机器*/
		if (errno == EPROTO || errno == ECONNABORTED){
			printf("超时。。。。\n");
			goto again;
		}
		else
			err_sys("accept error ");
	}
	alarm(0);
	return n;
}
ssize_t Read(int fd, void *ptr, size_t nbytes)
{
	ssize_t n;
	if ( (n = read(fd, ptr, nbytes)) == -1)
		err_sys("read error");
	return n;
}
size_t /* Write "n" bytes to a descriptor. */
writen(int fd, const void *vptr, size_t n)
{
	size_t nleft;
	ssize_t nwritten;
	const char *ptr;

	ptr = (const char *)vptr;
	nleft = n;
	while (nleft > 0)
	{
		if ((nwritten = write(fd, ptr, nleft)) <= 0)
		{
			if (nwritten < 0 && errno == EINTR)
				nwritten = 0; /* and call write() again */
			else
				return (-1); /* error */
		}

		nleft -= nwritten;
		ptr += nwritten;
	}
	return (n);
}
void Writen(int fd, void *ptr, size_t nbytes)
{
	if (writen(fd, ptr, nbytes) != nbytes)
		err_sys("writen error");
}
void Close(int fd)
{
	if (close(fd) == -1)
		err_sys("close error");
}

#endif
