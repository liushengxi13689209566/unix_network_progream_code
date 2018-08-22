#include "unp.h"

int main(int argc, char **argv)
{
	int sockfd, n;
	char recvline[MAXLINE + 1];
	socklen_t len;
	struct sockaddr_storage ss;

	if (argc != 3)
		err_quit("usage: daytimetcpcli <hostname/IPaddress> <service/port#>");

	sockfd = Tcp_connect(argv[1], argv[2]);

	len = sizeof(ss);
	Getpeername(sockfd, (SA *)&ss, &len);
	printf("connected to %s\n", Sock_ntop_host((SA *)&ss, len));
	int num ;
	for (;;)
	{
		/*MSG_PEEK 查看有多少数据但是不进行读取*/
		if ((n = recv(sockfd, recvline, MAXLINE, MSG_PEEK)) == 0)
			break;
		/*得到缓冲区里有多少字节要被读取，然后将字节数放入 b 里面。*/
		Ioctl(sockfd, FIONREAD, &num);
		printf("%d bytes from PEEK,%d bytes pending \n", n, num);
		n = Read(sockfd, recvline, MAXLINE);
		recvline[n] = 0; /* null terminate */
		Fputs(recvline, stdout);
	}
	exit(0);
}
