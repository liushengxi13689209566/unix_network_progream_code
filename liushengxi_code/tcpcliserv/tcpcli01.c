#include "../myhead.h"

void fun_client(int connfd)
{
	char sendline[MAXLINE] = {0};
	char recvline[MAXLINE] = {0};
	int tt = 0;
	while (Fgets(sendline, MAXLINE, stdin) != NULL)
	{
		tt = Sendlen(connfd, sendline, strlen(sendline), 0);
		/*阻塞于 Recvline 函数 */
//		if (Recvline(connfd, recvline, sizeof(recvline), 0) == 0)

        tt = recv(connfd,recvline,sizeof(recvline),0);
        //printf("tt == %d \n",tt);

		Fputs(recvline, stdout);
	}
}
int main(int argc, char **argv)
{
	int sockfd;
	struct sockaddr_in servaddr;
    const char *ip = argv[1];
    const int port =  atoi(argv[2]);

	//if (argc != 2)
	//{
	//	printf("usage: tcpcli <IPaddress>\n");
	//	return -1;
	//}

	sockfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);
	inet_pton(AF_INET, ip, &servaddr.sin_addr);

	connect(sockfd, (SA *)&servaddr, sizeof(servaddr));

	fun_client(sockfd);

	exit(0);
}
