#include "ping.h"

struct proto proto_v4 = {proc_v4, send_v4, NULL, NULL, NULL, 0, IPPROTO_ICMP};

int datalen = 56; /* data that goes with ICMP echo request */

int main(int argc, char **argv)
{
	struct addrinfo *ai;
	char *h;

	if (argc != 2)
		err_quit("use : ping <hostname> ");
	host = argv[1];

	pid = getpid() & 0xffff; /* ICMP ID field is 16 bits */
	Signal(SIGALRM, sig_alrm);

	ai = Host_serv(host, NULL, 0, 0);

	h = Sock_ntop_host(ai->ai_addr, ai->ai_addrlen);
	printf("PING %s (%s): %d data bytes\n",
		   ai->ai_canonname ? ai->ai_canonname : h,
		   h, datalen);

	/* 4initialize according to protocol */
	if (ai->ai_family == AF_INET)
		pr = &proto_v4;
	else if (ai->ai_family == AF_INET6)
		err_quit("暂时不支持 IPV6");
	else
		err_quit("unknown address family %d", ai->ai_family);

	pr->sasend = ai->ai_addr;
	pr->sarecv = malloc(sizeof(ai->ai_addrlen));
	pr->salen = ai->ai_addrlen;

	readloop();

	exit(0);
}
