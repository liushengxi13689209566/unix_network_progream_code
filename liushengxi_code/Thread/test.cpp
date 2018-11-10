/*************************************************************************
	> File Name: test.cpp
	> Author: Liu Shengxi 
	> Mail: 13689209566@163.com
	> Created Time: 2018年10月15日 星期一 09时33分43秒
 ************************************************************************/
#include <iostream>
#include <utility>
#include <thread>
#include <chrono>
#include <pthread.h>
class Task
{
    public:

    Task(){
        std::thread t2(f2, this); //
        t2.join();
    }
    static void f2(void *arg)
    {
        for (int i = 0; i < 5; ++i)
        {
            std::cout << "Thread 2 executing\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
};

int main()
{
    Task task ;
    std::cout << "Final value of n is "  << '\n';
}
