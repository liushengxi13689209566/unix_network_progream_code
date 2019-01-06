/*************************************************************************
	> File Name: lst_timer.h
	> Author: Liu Shengxi 
	> Mail: 13689209566@163.com
	> Created Time: 2019年01月01日 星期二 17时13分31秒
 ************************************************************************/

#ifndef _LST_TIMER_H
#define _LST_TIMER_H
#include "timer.h"
#include <queue>
#include <memory>
#include <vector>

class ListTimer
{
  public:
	using TimerPtr = std::shared_ptr<class Timer>;
	ListTimer(/* args */)
	{
	}
	~ListTimer()
	{
		auto size = que.size();
		for (int i = 0; i < size; i++)
		{
			que.pop();
		}
	}
	/*添加定时器*/
	void AddTimer(const TimerPtr &timer)
	{
		if (!timer)
			return;
		que.push(timer);
	}
	/*删除定时器（肯定是优先队列的第一个元素）想想为什么？？*/
	void DelTimer(const TimerPtr &timer)
	{
		if (!timer)
			return;
		que.pop(timer);
	}
	/*SIGALRM 信号被触发，就在信号处理函数中执行一次tick函数，以处理到期的任务*/
	void tick()
	{
		}

  private:
	/* data */
	std::priority_queue<TimerPtr, std::vector<TimerPtr>, std::greater<TimerPtr>> que;
};

#endif
