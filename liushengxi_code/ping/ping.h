/*************************************************************************
	> File Name: ping.h
	> Author: Liu Shengxi 
	> Mail: 13689209566@163.com
	> Created Time: 2018年10月09日 星期二 11时19分48秒
 ************************************************************************/

#ifndef _PING_H
#define _PING_H
#include "../myhead.h"
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>

#define BUFSIZE 1500
char sendbuf[BUFSIZE];
int datalen;
char *host;
int nsent;
pid_t pid;
int sockfd;

void init_v6(void);
void proc_v4(char *, ssize_t, struct msghdr *, struct timeval *);
void send_v4(void);
void readloop(void);
void sig_alrm(int);
void tv_sub(struct timeval *, struct timeval *);

struct proto
{
	void (*fproc)(char *, ssize_t, struct msghdr *, struct timeval *);
	void (*fsend)(void);
	void (*finit)(void);

	struct sockaddr *sasend;
	struct sockaddr *sarecv;
	socklen_t salen;
	int icmpproto;
} *pr;

#endif 