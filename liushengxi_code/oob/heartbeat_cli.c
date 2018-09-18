/*************************************************************************
	> File Name: heartbeat.c
	> Author: Liu Shengxi 
	> Mail: 13689209566@163.com
	> Created Time: 2018年09月17日 星期一 23时29分14秒
 ************************************************************************/

#include "../myhead.h"
#include"test.h"

static int servfd;
static int nsec;
static int maxnprobes; // probe:探头; 探索; 探针( probe的名词复数 ); 探测仪;
static int nprobes;						 //统计 SIGALRM 数量
static void sig_urg(int), sig_alrm(int); // alarm 是为了轮询

void heartbeat_cli(int servfd_arg, int nsec_arg, int maxnprobes_arg) //fd  1 5
{
	servfd = servfd_arg;
	nsec = nsec_arg;
	maxnprobes = maxnprobes_arg;
	nprobes = 0;
	signal(SIGURG, sig_urg);
	Fcntl(servfd, F_SETOWN, getpid());

	signal(SIGALRM, sig_alrm);
	alarm(nsec);
}

static void sig_urg(int signo)
{
	int n;
	char ch;
	if ((n = recv(servfd, &ch, 1, MSG_OOB)) < 0)//只要产生带外数据，就说明服务器主机是存活的
	{
		if (errno != EWOULDBLOCK)
			err_sys("revc error");
	}
	nprobes = 0;
	return;
}

static void sig_alrm(int signo)
{
	if (++nprobes > maxnprobes)
	{
		fprintf(stderr, "server is unreachable\n");
		exit(0);
	}
	send(servfd, "1", 1, MSG_OOB);
	alarm(nsec);
	return;
}