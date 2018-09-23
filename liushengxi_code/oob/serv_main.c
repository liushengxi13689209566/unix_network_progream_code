

#include "../myhead.h"
#include "test.h"

void fun_serv(int connfd) //子进程运行函数
{
    ssize_t n;
    char line[MAXLINE];

    heartbeat_serv(connfd, 1, 5);

    for (;;)
    {
        if ((n = Recvline(connfd, line, MAXLINE, 0)) == 0)
        {
            printf("客 户 端 关  闭 啦 ！！！\n");
            Close(connfd);
            return ; 
        }
        Sendlen(connfd, line, n, 0);
    }
}

int main(int argc, char **argv)
{
    int listenfd, connfd;
    pid_t childpid;
    socklen_t clilen;
    struct sockaddr_in cliaddr, servaddr;

    listenfd = Socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT); //9877

    int opt = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (int *)&opt, sizeof(int));

    Bind(listenfd, (SA *)&servaddr, sizeof(servaddr));

    Listen(listenfd, LISTENQ);

    for (;;)
    {
        clilen = sizeof(cliaddr);
        if ((connfd = Accept(listenfd, (SA *)&cliaddr, &clilen)) < 0)
        {
            if (errno == EINTR)
                continue;
            else
                err_sys("accept error ");
        }

        if ((childpid = Fork()) == 0)
        {                    
            Close(listenfd); 

            printf(" 新的连接：connfd == %d \n", connfd);

            fun_serv(connfd); 

            exit(0);
        }
        Close(connfd); 
    }
}
