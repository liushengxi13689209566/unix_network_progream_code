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
#endif
