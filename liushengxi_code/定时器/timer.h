/*************************************************************************
	> File Name: timer.h
	> Author: Liu Shengxi 
	> Mail: 13689209566@163.com
	> Created Time: 2019年01月01日 星期二 17时16分49秒
 ************************************************************************/

#ifndef _TIMER_H
#define _TIMER_H
#include <time.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
const int BUFFER_SIZE = 64;

class Timer;
/*用户数据结构*/
class client_data
{
  public:
	struct sockaddr_in address;
	int sockfd;
	char buf[BUFFER_SIZE];
	Timer *timer;
};
/*定时器类*/
class Timer
{
  public:
	Timer(/* args */)
	{
	}
	~Timer()
	{
	}
	bool operator>(const Timer a) const //重载　＞　
	{
		return expire > a.expire;
	}

	time_t expire;					//任务的超时事件
	void (*cb_func)(client_data *); // 回调函数
	client_data *user_data;			//回调函数处理的客户数据，由定时器的执行者传递给回调函数
};
#endif
