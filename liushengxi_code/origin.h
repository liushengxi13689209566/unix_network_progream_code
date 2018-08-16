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
#include <signal.h>
#define MAXLINE 1024
#define SA struct sockaddr
#define SERV_PORT 9877
#define LISTENQ 1024
typedef void Sigfunc(int);

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
	char *ptr = NULL;
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
Sigfunc *
Signal(int signo, Sigfunc *func) /* for our signal() function */
{
	Sigfunc *sigfunc;

	if ((sigfunc = signal(signo, func)) == SIG_ERR)
		err_sys("signal error");
	return (sigfunc);
}
static void
sig_alrm(int signo)
{
	return; /* just interrupt the recvfrom() */
}
int Accept(int fd, struct sockaddr *sa, socklen_t *salenptr, int nsec)
{
	printf("Accept\n");

	int n;
	Signal(SIGALRM, sig_alrm);
	alarm(nsec); //设置时间

again:
	if ((n = accept(fd, sa, salenptr)) < 0)
	{
		printf("vbkrvbk");
		if (errno == EINTR)
		{
			printf("超时。。。。。\n");
		}
	}
	else
	{
		alarm(0);
	}
	goto again;
}
ssize_t Read(int fd, void *ptr, size_t nbytes)
{
	ssize_t n;
	if ((n = read(fd, ptr, nbytes)) == -1)
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
pid_t Fork(void)
{
	pid_t pid;

	if ((pid = fork()) == -1)
		err_sys("fork error");
	return (pid);
}
ssize_t /* Read "n" bytes from a descriptor. */
readn(int fd, void *vptr, size_t n)
{
	size_t nleft;
	ssize_t nread;
	char *ptr;

	ptr = vptr;
	nleft = n;
	while (nleft > 0)
	{
		if ((nread = read(fd, ptr, nleft)) < 0)
		{
			if (errno == EINTR)
				nread = 0; /* and call read() again */
			else
				return (-1);
		}
		else if (nread == 0)
			break; /* EOF */

		nleft -= nread;
		ptr += nread;
	}
	return (n - nleft); /* return >= 0 */
}
/* end readn */

#endif
