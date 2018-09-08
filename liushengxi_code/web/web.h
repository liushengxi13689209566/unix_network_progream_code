/*************************************************************************
	> File Name: web.h
	> Author: Liu Shengxi 
	> Mail: 13689209566@163.com
	> Created Time: 2018年08月24日 星期五 10时52分30秒
 ************************************************************************/

#ifndef _WEB_H
#define _WEB_H

#include "../myhead.h"

#define MAXFILES 20
#define SERV "80"
struct file
{
	char *f_name;
	char *f_host;
	int f_fd;
	int f_flags;
}
file[MAXFILES];

#define F_CONNECTING 1
#define F_READING 2
#define F_DONE 4

#define GET_CMD "GET %s HTTP/1.0\r\n\r\n"

int nconn, nfiles, nlefttoconn, nlefttoread, maxfd;
fd_set rset, wset;
/*
nconn:当前打开的连接数，不超过第一个命令行参数
nlefttoread:待读取的文件数量
nlefttoconn:尚未连接的文件数
nfiles:文件数量
*/
ssize_t /* Write "n" bytes to a descriptor. */
writen(int fd, const void *vptr, size_t n)
{
	size_t nleft;
	ssize_t nwritten;
	const char *ptr;

	ptr = vptr;
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
/* end writen */

void Writen(int fd, void *ptr, size_t nbytes)
{
	if (writen(fd, ptr, nbytes) != nbytes)
		err_sys("writen error");
}

#endif
