/*************************************************************************
	> File Name: base.h
	> Author: Liu Shengxi 
	> Mail: 13689209566@163.com
	> Created Time: 2019年01月06日 星期日 17时07分53秒
 ************************************************************************/

#ifndef _BASE_H
#define _BASE_H
#include <ctime>
class Node
{
  public:
	time_t data;
	bool operator>(const Node a) const //重载　＞　
	{
		return data > a.data;
	}
};
#endif
