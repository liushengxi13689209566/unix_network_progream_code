/*************************************************************************
	> File Name: main.cpp
	> Author: Liu Shengxi 
	> Mail: 13689209566@163.com
	> Created Time: 2018年12月31日 星期一 21时35分13秒
 ************************************************************************/

#include <iostream>
#include "socket.h"
#include "epoll.h"
using namespace std;

int listenfd = -1;
static int pipefd[2];

void SetNonBlock(const int &fd)
{
	int old_option = fcntl(fd, F_GETFL);
	int new_option = old_option | O_NONBLOCK;
	fcntl(fd, F_SETFL, new_option);
}
void sig_handler(int sig)
{
	int save_errno = errno;
	int msg = sig;
	send(pipefd[1], (char *)&msg, 1, 0); /*将信号值写入管道，通知主循环*/
	errno = save_errno;
}
void addsig(int sig)
{

	struct sigaction sa;
	memset(&sa, '\0', sizeof(sa));
	sa.sa_handler = sig_handler;
	/*由此信号中断的系统调用是否要再启动*/
	sa.sa_flags |= SA_RESTART;

	/*sigfillset()用来将参数set 信号集初始化, 然后把所有的信号加入到此信号集里*/
	sigfillset(&sa.sa_mask);
	assert(sigaction(sig, &sa, NULL) != -1);
}
int main(int argc, char *argv[])
{
	if (argc <= 2)
	{
		printf("usage: %s ip_address port_number\n", basename(argv[0]));
		return 1;
	}
	const char *ip = argv[1];
	const int port = atoi(argv[2]);

	ServSocket server(ip, port);
	server.Bind();
	server.Listen();
	Epoll epoll;

	listenfd = server.GetBaseSocket();
	epoll.Add(listenfd);
	SetNonBlock(listenfd);
	auto ret = socketpair(PF_UNIX, SOCK_STREAM, 0, pipefd);
	assert(ret != -1);
	//pipefd[0] 用来读　pipefd[1]　用来写
	// 将信号注册进pipe管道的读端pipefd[0]，通过对读端的监听，来实现统一事件源。
	SetNonBlock(pipefd[1]);
	epoll.Add(pipefd[0]);

	addsig(SIGHUP);
	addsig(SIGCHLD);
	addsig(SIGTERM);
	addsig(SIGINT);
	while (true)
	{
		int number = epoll.Wait();
		for (int i = 0; i < number; i++)
		{
			int sockfd = epoll.GetFdByIndex(i);
			if (sockfd == listenfd)
			{
				struct sockaddr_in cli_addr;
				socklen_t len = sizeof(cli_addr);
				int connfd = server.Accept(reinterpret_cast<struct sockaddr *>(&cli_addr), &len);
			}
			else if (sockfd == pipefd[0] && (epoll.GetEventsByIndex(i) & EPOLLIN))
			{
				int sig;
				char signals[1024];
				ret = recv(pipefd[0], signals, sizeof(signals), 0);
				if (ret == -1)
					continue;
				else if (ret == 0)
					continue;
				else
				{
					/*每个信号占一个字节，所以可以按照字节来逐个接受信号*/
					for (int i = 0; i < ret; i++)
					{
						switch (signals[i])
						{
						case SIGCHLD:
							cout << "SIGSHLD signal" << endl;
							break;
						case SIGHUP:
							cout << "SIGHUP signal" << endl;
							break;
						case SIGTERM:
							cout << "SIGTERM signal" << endl;
							break;
						case SIGINT:
							cout << "SIGINT signal" << endl;
							break;
						}
					}
				}
			}
			else
			{
			}
		}
	}
	close(pipefd[1]);
	close(pipefd[0]);
	return 0;
}
