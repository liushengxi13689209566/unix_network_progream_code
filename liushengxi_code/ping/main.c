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
	if ((out->tv_usec -= in->tv_usec) < 0)
	{
		--out->tv_sec;
		out->tv_usec += 1000000;
	}
	out->tv_sec -= in->tv_sec;
}
struct addrinfo *
Host_serv(const char *host, const char *serv, int family, int socktype)
{
	int n;
	struct addrinfo hints, *res;

	bzero(&hints, sizeof(struct addrinfo));
	hints.ai_flags = AI_CANONNAME; /* always return canonical name */
	hints.ai_family = family;	  /* 0, AF_INET, AF_INET6, etc. */
	hints.ai_socktype = socktype;  /* 0, SOCK_STREAM, SOCK_DGRAM, etc. */

	if ((n = getaddrinfo(host, serv, &hints, &res)) != 0)
		err_quit("host_serv error for %s, %s: %s",
				 (host == NULL) ? "(no hostname)" : host,
				 (serv == NULL) ? "(no service name)" : serv,
				 gai_strerror(n));

	return (res); /* return pointer to first on linked list */
}
uint16_t
in_cksum(uint16_t *addr, int len)
{
	int nleft = len;
	uint32_t sum = 0;
	uint16_t *w = addr;
	uint16_t answer = 0;
	while (nleft > 1)
	{
		sum += *w++;
		nleft -= 2;
	}
	/* 4mop up an odd byte, if necessary */
	if (nleft == 1)
	{
		*(unsigned char *)(&answer) = *(unsigned char *)w;
		sum += answer;
	}
	/* 4add back carry outs from top 16 bits to low 16 bits */
	sum = (sum >> 16) + (sum & 0xffff); /* add hi 16 to low 16 */
	sum += (sum >> 16);					/* add carry */
	answer = ~sum;						/* truncate to 16 bits */
	return (answer);
}
void sig_alrm(int signo){
	(*pr->fsend)();
	alarm(1);
	return ;
}
char *Sock_ntop_host(const struct sockaddr *sa, socklen_t salen)
{
	static char str[128]; 
	switch (sa->sa_family)
	{
	case AF_INET:
	{
		struct sockaddr_in *sin = (struct sockaddr_in *)sa;

		if (inet_ntop(AF_INET, &sin->sin_addr, str, sizeof(str)) == NULL)
			return (NULL);
		return (str);
	}
	}
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
	if (ip->ip_p != IPPROTO_ICMP)
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
		tv_sub(tvrecv, tvsend);
		rtt = tvrecv->tv_sec * 1000.0 + tvrecv->tv_usec / 1000.0;

		printf("%d bytes form %s : seq=%u,ttl=%d,rtt=%.3f ms\n",
			   icmplen, Sock_ntop_host(pr->sarecv, pr->salen), icmp->icmp_seq, ip->ip_ttl, rtt);
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

	sockfd = Socket(pr->sasend->sa_family, SOCK_RAW, pr->icmpproto);
	setuid(getuid()); //不再需要特殊权限
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
		gettimeofday(&tval, NULL);
		(*pr->fproc)(recvbuf, n, &msg, &tval);
	}
}
void send_v4()
{
	int len;
	struct icmp *icmp;
	
	icmp = (struct icmp *)sendbuf;
	icmp->icmp_type = ICMP_ECHO;
	icmp->icmp_code = 0;
	icmp->icmp_id = pid;
	icmp->icmp_seq = nsent++;
	memset(icmp->icmp_data, 0xa5, datalen);
	gettimeofday((struct timeval *)icmp->icmp_data, NULL);

	len = 8 + datalen;
	icmp->icmp_cksum = 0;
	icmp->icmp_cksum = in_cksum((u_short *)icmp, len);

	sendto(sockfd, sendbuf, len, 0, pr->sasend, pr->salen);
}
int main(int argc, char **argv)
{
	int c;
	struct addrinfo *ai;
	char *h;

	if (argc != 2)
		err_quit("use : ping <hostname> ");
	host = argv[1];

	pid = getpid() & 0xffff; //  ICMP 16位
	signal(SIGALRM, sig_alrm);

	ai = Host_serv(host, NULL, 0, 0);
	h = Sock_ntop_host(ai->ai_addr, ai->ai_addrlen); //将主机名转换为`ip`地址的功能,使用更加通用的协议 ipv6,简单一点就是 gethostbyname
	printf("PING %s (%s):%d data bytes \n", ai->ai_canonname ? ai-> ai_canonname : h, h, datalen);
	if (ai->ai_family == AF_INET)
		pr = &proto_v4;
	pr->sasend = ai->ai_addr;
	pr->sarecv = malloc(sizeof(ai->ai_addrlen));

	readloop();

	exit(0);
}
