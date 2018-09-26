
#include "../myhead.h"

void *copyto(void *);
int global_fd;
FILE *fp;

void handle(FILE *fp_arg, int global_fd_arg)
{
    char recvline[MAXLINE];
    pthread_t tid;
    global_fd = global_fd_arg;
    fp = fp_arg;
    pthread_create(&tid, NULL, copyto, NULL);

    while (Recvline(global_fd, recvline, MAXLINE, 0) > 0)
        Fputs(recvline, stdout);
}
void *copyto(void *arg)
{
    char sendline[MAXLINE];

    while (Fgets(sendline, MAXLINE, fp) != NULL)
        Sendlen(global_fd, sendline, strlen(sendline), 0);

    shutdown(global_fd, SHUT_WR);

    return (NULL);
}


int main(int argc, char **argv)
{
    int sockfd;
    struct sockaddr_in servaddr;

    if (argc != 2)
    {
        printf("usage: tcpcli <IPaddress>\n");
        return -1;
    }

    sockfd = Socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

    connect(sockfd, (SA *)&servaddr, sizeof(servaddr));

    handle(stdin,sockfd);

    exit(0);
}
