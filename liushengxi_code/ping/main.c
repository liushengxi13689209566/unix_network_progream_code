/*************************************************************************
	> File Name: main.c
	> Author: Liu Shengxi 
	> Mail: 13689209566@163.com
	> Created Time: 2018年10月09日 星期二 11时30分43秒
 ************************************************************************/

#include "ping.h"

struct proto proto_v4 = {proc_v4, send_v4, NULL, NULL, NULL, 0, IPPROTO_ICMP};

int datalen = 56;

// 辅助函数
void tv_sub(struct timeval *out, struct timeval *in)
{
	if ((out->tv_usec -= in_tv_usec) < 0)
	{
		--out->tv_sec;
		out->tv_usec += 1000000;
	}
	out->tv_sec -= in->tv_sec;
}
// 处理接受到的 ICMPv4 消息
void proc_v4(char *ptr, ssize_t len, struct msghdr *msg, struct timeval *tvrecv)
{
	int hlen1, icmplen;
	double rtt;

	struct ip *ip;
	struct icmp *icmp;

	struct timeval *tvsend ;
	ip = (struct ip *)ptr;
	hlen1 = ip->ip_hl << 2 ;

}
// 主函数
void readloop()
{
	int size;
	char recvbuf[BUFSIZE];
	char controlbuf[BUFSIZE];
	struct msghdr msg;
	struct iovec iov;
	ssize_t n;
	struct timeval tval;

	sockfd = Socket(pr->sasend->sa_family, SOCK_RAM, pr->icmpproto);
	steuid(getuid()); //不再需要特殊权限
	size = 60 * 1024;
	setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &size, sizeof(size));

	sig_alrm(SIGALRM);

	iov.iov_base = recvbuf;
	iov.iov_len = sizeof(recvbuf);
	msg.msg_name = pr->sarecv;
	msg.msg_iov = &iov;
	msg.msg_control = controlbuf;
	while (1)
	{
		msg.msg_namelen = pr->salen;
		msg.msg_controllen = sizeof(controlbuf);
		n = recvmsg(sockfd, &msg, 0); // 接 受 数 据
		if (n < 0)
		{
			if (errno == EINTR)
				continue;
			else
				err_sys("recvmsg error ");
		}
		Gettimeofday(&tval, NULL);
		(*pr->fproc)(recvbug, n, &msg, &tval);
	}
}
int main(int argc, char **argv)
{
	int c;
	struct addrinfo *ai;
	char *h;
	opterr = 0; //不希望getopt()印出错信息，则只要将全域变量opterr 设为0 即可。
	while ((c = getopt(argc, argv, "v")) != -1)
	{
		printf("c== %c \n", c);
		switch (c)
		{
		case 'c':
			verbose++;
			break;
		case '?':
			err_quit("不支持的标志 : %c ", c);
		}
	}
	if (optind != argc - 1)
		err_quit("use : ping [ -v ] <hostname> ");
	host = argv[optind];
	pid = getpid() & 0xffff; //  ICMP 16位
	signal(SIGALRM, sig_alrm);

	ai = Host_serv(host, NULL, 0, 0);
	h = Sock_ntop_host(ai->ai_addr, ai->ai_addrlen);
	printf("PING %s (%s):%d data bytes \n", ai->ai_canonname ? ai_ > ai_canonname : h, h, datelen);
	if (ai->ai_family == AF_INET)
		pr = &proto_v4;
	pr->sasend = ai->ai_addr;
	pr->sarecv = malloc(sizeof(ai->ai_addrlen));

	readloop();

	exit(0);
}