#include "unp.h"

static int servfd;
static int nsec;	   /* #seconds betweeen each alarm */
static int maxnprobes; /* #probes w/no response before quit */
static int nprobes;	/* #probes since last server response */
static void sig_urg(int), sig_alrm(int);

void heartbeat_cli(int servfd_arg, int nsec_arg, int maxnprobes_arg)
{
	servfd = servfd_arg; /* set globals for signal handlers */
	if ((nsec = nsec_arg) < 1)
		nsec = 1;
	if ((maxnprobes = maxnprobes_arg) < nsec)
		maxnprobes = nsec;
	nprobes = 0;

	Signal(SIGURG, sig_urg);
	Fcntl(servfd, F_SETOWN, getpid());

	Signal(SIGALRM, sig_alrm);
	alarm(nsec);
}

static void
sig_urg(int signo)
{
	printf("产生  SIGURG 信号 \n");
	int n;
	char c;

	if ((n = recv(servfd, &c, 1, MSG_OOB)) < 0)
	{
		if (errno != EWOULDBLOCK)
			err_sys("recv error");
	}
	else if (n > 0)
	{
		printf("客户端接收到带外数据，说明服务器主机是存活的\n");
		nprobes = 0;
	}
	return; /* may interrupt client code */
}

static void
sig_alrm(int signo)
{
	if (++nprobes > maxnprobes)
	{
		fprintf(stderr, "server is unreachable\n");
		exit(0);
	}
	if (send(servfd, "1", 1, MSG_OOB) > 0)
		printf("客户端发送了带外数据\n");

	alarm(nsec);
	return; /* may interrupt client code */
}
