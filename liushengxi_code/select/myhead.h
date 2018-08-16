/*************************************************************************
	> File Name: myhead.h
	> Author: Liu Shengxi
	> Mail: 13689209566@163.com
	> Created Time: 2018年08月14日 星期二 15时02分03秒
 ************************************************************************/

#ifndef _MYHEAD_H
#define _MYHEAD_H
#define LISTENQ 1024
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
class mySelect
{
  public:
	mySelect(const char *_ip, const int _port) : ip(_ip), port(_port) { run(ip, port); }
	~mySelect();
	int run(const char *ip, const int port);

  private:
	const int port;
	const char *ip;
	/*（maxfd+1）是select函数的第一个参数的当前值
	listenfd 是监听套接字
	maxi 是client 数组当前使用项的最大下标
	*/
	int listenfd, maxfd, maxi;
	int client[FD_SETSIZE]; //保存已连接套接字，刚开始初始化为 -1
	fd_set rset, allset;
	int nready;
};
#endif
