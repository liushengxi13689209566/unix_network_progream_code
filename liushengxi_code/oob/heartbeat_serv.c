#include "../myhead.h"

static int servfd;
static int nsec;
static int maxnalarms;
static int nprobes;                      //统计 SIGALRM 数量
static void sig_urg(int), sig_alrm(int); // alarm 是为了轮询

void heartbeat_serv(int servfd_arg, int nsec_arg, int maxnalarms_arg) //fd  1 5
{
    servfd = servfd_arg;
    nsec = nsec_arg;
    maxnalarms = maxnalarms_arg;
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
    if ((n = recv(servfd, &ch, 1, MSG_OOB)) < 0) //只要产生带外数据，就说明服务器主机是存活的
    {
        if (errno != EWOULDBLOCK)
            err_sys("revc error");
    }
    send(servfd, &ch, 1, MSG_OOB);
    nprobes = 0;
    return;
}
static void sig_alrm(int signo)
{
    if (++nprobes > maxnalarms)
    {
        fprintf(stderr, "no probes form cliient \n");
        exit(0);
    }
    alarm(nsec);
    return;
}