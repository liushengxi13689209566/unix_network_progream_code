/*************************************************************************
	> File Name: testThreadPool.cpp
	> Author: Liu Shengxi 
	> Mail: 13689209566@163.com
	> Created Time: 2018年10月15日 星期一 09时12分21秒
 ************************************************************************/

#include "threadPool_2.h"
using namespace std;
class Test
{
  public:
	void process_no_static_bind(const int i, const int j) /*推荐使用*/
	{
		cout << "bind:  i==" << i << " "
			 << "j==" << j << endl;
	}
};
int main(void)
{
	threadPool pool(6);
	Test tt_bind;
	while (true)
	{
		pool.append(std::bind(&Test::process_no_static_bind, &tt_bind, 3, 4));
	}
}
