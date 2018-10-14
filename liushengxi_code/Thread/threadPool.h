/*************************************************************************
	> File Name: threadPool.h
	> Author: Liu Shengxi 
	> Mail: 13689209566@163.com
	> Created Time: 2018年10月14日 星期日 21时52分53秒
 ************************************************************************/

#ifndef _THREADPOOL_H
#define _THREADPOOL_H
#include <vector>
#include <queue>
#include <thread>
#include <functional>

const int MAX_REQUESTS = 20000; // 最大请求数目
const int MAX_THREADS = 1000;   //最大线程数目

template <typename T>
class threadPool
{
  public:
	threadPool(int number = 1); //默认开一个线程
	~threadPool();
	bool append(T *request);
	static void *worker(void *arg);
	void run();

  private:
	std::vector<std::thread> work_threads;
	std::queue<T *> tasks_queue;

	std::mutex queue_mutex;
	std::condition_variable condition;
	bool stop;
};
template <typename T>
threadPool<T>::threadPool(int number) : stop(nullptr)
{
	if (number <= 0 || number > MAX_THREADS)
		throw std::exception();

	for (size_t i = 0; i < number; i++)
	{
		std::thread temp(worker, this);
		work_threads.emplace_back(temp);
		temp.detach();
	}
}

template <typename T>
threadPool<T>::~threadPool()
{
	work_threads.clear();
	stop = true;
}

template <typename T>
bool threadPool<T>::append(T *request)
{
	/*操作工作队列时一定要加锁，因为他被所有线程共享*/
	queue_mutex.lock();
	if (tasks_queue.size() > MAX_REQUESTS)
	{
		queue_mutex.unlock();
		return false;
	}
	tasks_queue.push(resuest);
	queue_mutex.unlock();
	condition.notify_one(); //线程池添加进去了任务，自然要通知等待的线程
	return true;
}
template <typename T>
void threadPool<T>::worker(void *arg)
{
	threadPool *pool = (threadPool *)arg;
	pool->run();
	return pool;
}
template <typename T>
void threadPool<T>::run()
{
	while (!stop)
	{
		std::unique_lock(std::mutex) lk(this->queue_mutex);
		this->condition.wait(lk, [this] { !this->tasks_queue.empty() }); //如果任务队列为空，就停下来等待
		
	}
}
#endif
