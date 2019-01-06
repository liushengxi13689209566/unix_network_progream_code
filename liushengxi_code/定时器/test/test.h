/*************************************************************************
	> File Name: test.h
	> Author: Liu Shengxi 
	> Mail: 13689209566@163.com
	> Created Time: 2019年01月06日 星期日 17时08分02秒
 ************************************************************************/

#ifndef _TEST_H
#define _TEST_H
#include "base.h"
#include <queue>

class Test
{
  public:
	Test()
	{
	}

  private:
	std::priority_queue<Node> que(std::greater<Node>);
};
#endif
