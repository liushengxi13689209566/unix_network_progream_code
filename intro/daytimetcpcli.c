#include "unp.h"

int sockfd_to_family(int sockfd)
{
	struct sockaddr_storage ss;
	socklen_t len;
	len = sizeof(ss);
	if (getsockname(sockfd, (struct sockaddr *)&ss, &len) < 0)
		return -1;
	return (ss.ss_family);
}
int main(int argc, char **argv)
{
	int sockfd, n;
	char recvline[2];
	struct sockaddr_in servaddr;

	if (argc != 3)
		err_quit("usage: a.out <IPaddress>   port");

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		err_sys("socket error");

	const int port = atoi(argv[2]);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port); /* daytime server */
	if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0)
		err_quit("inet_pton error for %s", argv[1]);

	int recvSize, sendSize, MSS;
	socklen_t len = sizeof(int);
	getsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &recvSize, &len);
	getsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, &sendSize, &len);

	printf("接受缓冲区大小：%d,发送缓冲区大小：%d ,MSS:未测 \n", recvSize, sendSize);

	if (connect(sockfd, (SA *)&servaddr, sizeof(servaddr)) < 0)
		err_sys("connect error");
	int sum = 0;
	getsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &recvSize, &len);
	getsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, &sendSize, &len);

	printf("接受缓冲区大小：%d,发送缓冲区大小：%d ,MSS：未测 \n", recvSize, sendSize);

	while ((n = read(sockfd, recvline, 1)) > 0)
	{
		recvline[n] = 0; /* null terminate */
		sum++;
		if (fputs(recvline, stdout) == EOF)
			err_sys("fputs error");
	}
	if (n < 0)
		err_sys("read error");
	printf("sum ==  %d \n", sum);
	printf("地址族是：%d \n", sockfd_to_family(sockfd));

	exit(0);
}
