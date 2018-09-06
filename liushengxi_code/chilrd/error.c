/*************************************************************************
	> File Name: error.c
	> Author: Liu Shengxi 
	> Mail: 13689209566@163.com
	> Created Time: 2018年09月06日 星期四 20时33分58秒
 ************************************************************************/

#include "../myhead.h"
#include <stdarg.h> //主要用于函数变参
#include <syslog.h> //系统错误日志记录的定义

int daemon_proc;

static void err_doit(int, int, const char *, va_list);

void err_ret(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	err_doit(1, LOG_INFO, fmt, ap);
	va_end(ap);
	return;
}
void err_sys(const char *fmt, ...){
	va_list ap;
	va_start(ap,fmt);
	err_doit(1,LOG_ERR,fmt,ap);
	va_end(ap);
	exit(1);
}

void err_dump(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	err_doit(1, LOG_ERR, fmt, ap);
	va_end(ap);
	abort();
	exit(1);
}
void err_quit(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	err_doit(0, LOG_ERR, fmt, ap);
	va_end(ap);
	exit(1);
}
void err_msg(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	err_doit(0, LOG_INFO, fmt, ap);
	va_end(ap);
	return;
}
static void err_doit(int errnoflag, int level, const char *fmt, va_list ap)
{
	int errno_save, n;
	char buf[MAXLINE + 1];
	errno_save = errno;
	/*vsnprintf，C语言库函数之一，属于可变参数。用于向字符串中打印数据、数据格式用户自定义*/
	vsnprintf(buf, MAXLINE, fmt, ap);
	n = strlen(buf);
	if (errnoflag)
		snprintf(buf + n, MAXLINE - n, ": %s", strerror(errno_save));
	strcat(buf, "\n");

	if (daemon_proc)
	{
		syslog(level, buf);
	}
	else
	{
		fflush(stdout);
		fputs(buf, stderr);
		fflush(stderr);
	}
	return;
}
