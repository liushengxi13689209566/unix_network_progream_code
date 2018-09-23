#include	"unp.h"

static int	servfd;
static int	nsec;			/* #seconds between each alarm */
static int	maxnalarms;		/* #alarms w/no client probe before quit */
static int	nprobes;		/* #alarms since last client probe */
static void	sig_urg(int), sig_alrm(int);

void
heartbeat_serv(int servfd_arg, int nsec_arg, int maxnalarms_arg)
{
	servfd = servfd_arg;		/* set globals for signal handlers */
	if ( (nsec = nsec_arg) < 1)
		nsec = 1;
	if ( (maxnalarms = maxnalarms_arg) < nsec)
		maxnalarms = nsec;

	Signal(SIGURG, sig_urg);
	Fcntl(servfd, F_SETOWN, getpid());

	Signal(SIGALRM, sig_alrm);
	alarm(nsec);
}

static void
sig_urg(int signo)
{
	printf("产生  SIGURG 信号 \n");
	int		n;
	char	c;
	
	sleep(6);
	
	if ( (n = recv(servfd, &c, 1, MSG_OOB)) < 0) {
		if (errno != EWOULDBLOCK)
			err_sys("recv error");
	}
	else if(n> 0)
	{
		printf("服务器接收到带外数据，说明客户端主机是存活的\n");
		Send(servfd, &c, 1, MSG_OOB); /* echo back out-of-band byte */
		printf("服务器发送了带外数据\n");
		nprobes = 0; /* reset counter */
	}
	return;					/* may interrupt server code */
}

static void
sig_alrm(int signo)
{
	if (++nprobes > maxnalarms) {
		printf("no probes from client\n");
		exit(0);
	}
	alarm(nsec);
	return;					/* may interrupt server code */
}
