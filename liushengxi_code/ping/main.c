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

	struct timeval *tvsend;
	ip = (struct ip *)ptr;
	hlen1 = ip->ip_hl << 2;
	if (ip->p != IPPROTO_ICMP)
		return; //不是icmp
	icmp = (struct icmp *)(ptr + hlen1);
	if ((icmplen = len - hlen1) < 8)
		return; //  ICMP 首部凑不齐，包异常
	if (icmp->icmp_type == ICMP_ECHOREPLY)
	{ //是 ICMP 回射应答
		if (icmp->icmp_id != pid)
			return; //说明不是本进程所需要的回应
		if (icmplen < 16)
			return; //没有足够的数据使用

		tvsend = (struct timeval *)icmp->icmp_data;
		tc_sub(tvrecv, tvsend);
		rtt = tvrecv->tv_sec * 1000.0 + tvrecv->usec / 1000.0;

		printf("%d bytes form %s : seq=%u,ttl=%d,rtt=%.3f ms\n",
		icmplen,Sock_ntop_host(pr->sarecv,pr->salen),icmp->icmp_seq,ip->ip_ttl,rtt);
	}
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

	if (argc != 2 )
		err_quit("use : ping <hostname> ");
	host = argv[1];
	
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