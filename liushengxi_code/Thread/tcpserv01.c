#include "../myhead.h"
static void *doit(void *);

void fun(int connfd)
{
    ssize_t n;
    char buf[1024] = {0};
    while (1)
    {
        bzero(buf, sizeof(buf));
        n = Recvline(connfd, buf, 1024, 0);

      //  printf("666\n");

        if (n <= 0)
        {
            printf("对端关闭\n");
            break;
        }
        Sendlen(connfd, buf, n, 0);
    }
}
int main(int argc, char **argv)
{
    int listenfd, connfd ;
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
    pthread_t tid;
    for (;;)
    {
        clilen = sizeof(cliaddr);
        connfd = Accept(listenfd, (SA *)&cliaddr, &clilen);

        printf("新的连接：connfd== %d \n", connfd);

        pthread_create(&tid, NULL, &doit, (void *)connfd);
    }
}
void *doit(void *arg)
{
    //printf("44444444444444444444444\n");
    int fd ;

    fd = (int)arg;

    pthread_detach(pthread_self()); //此线程与上一个线程无关啦！！！
    //printf("33333333333\n");

    fun(fd);

    Close(fd);
    return (NULL);
}
