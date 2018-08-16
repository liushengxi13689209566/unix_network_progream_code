/*************************************************************************
	> File Name: client.cpp
	> Author: Liu Shengxi 
	> Mail: 13689209566@163.com
	> Created Time: 2018年08月14日 星期二 21时57分02秒
 ************************************************************************/

#include "origin.h"
int main(int argc, char **argv)
{
	int sockfd;
	struct sockaddr_in servaddr;

	if (argc != 2)
		err_sys("usage: tcpcli <IPaddress>");

	sockfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	int port = 9877;
	servaddr.sin_port = htons(port);
	inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

	connect(sockfd, (SA *)&servaddr, sizeof(servaddr));
    char buf[MAXLINE];
    while(1){

        while(read(sockfd,buf,MAXLINE) > 0)	
            printf("%s\n",buf);
    }

	exit(0);
}
