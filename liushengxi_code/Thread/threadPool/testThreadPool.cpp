/*************************************************************************
	> File Name: testThreadPool.cpp
	> Author: Liu Shengxi 
	> Mail: 13689209566@163.com
	> Created Time: 2018年10月15日 星期一 09时12分21秒
 ************************************************************************/

#include "threadPool.h"
#include<string>
using namespace std;
class Task
{
	public:
	void process()
	{
		cout << "run........." << endl;
	}
};
int main(void)
{
	threadPool<Task> pool(6);
    std::string str;
	while (1)
	{
			Task *tt = new Task();
			//使用智能指针
			pool.append(tt);
            delete tt;
    }
}
