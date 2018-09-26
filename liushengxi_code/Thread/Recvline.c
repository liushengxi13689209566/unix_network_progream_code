#include "../myhead.h"

static pthread_key_t rl_key;
static pthread_once_t rl_once = PTHREAD_ONCE_INIT; // 0

static void recvline_destructor(void *ptr)
{
    free(ptr);
}
static void recvline_once(void)
{
    pthread_key_create(&rl_key, recvline_destructor);
}
typedef struct
{
    int rl_cnt;
    char *rl_bufptr;
    char rl_buf[MAXLINE];
} Rline;

static ssize_t my_recv(Rline *tsd, int fd, char *ptr)
{
    if (tsd->rl_cnt <= 0)
    {
    again:
        if ((tsd->rl_cnt = recv(fd, tsd->rl_buf, MAXLINE, 0)) < 0)
        {
            if (errnp == EINTR)
                goto again;
            else
                return (-1);
        }
        tsd->rl_bufptr = tsd->rl_buf;
    }
    else
    {
        tsd->rl_cnt--;
        *ptr = *tsd->bufptr++;
        return (1);
    }
}
ssize_t recvline(int fd, void *vptr, size_t maxlen)
{
    size_t n, rc;
    char ch, *ptr;
    Rline tsd;
    pthread_once(&rl_once, recvline_once);
    if ((tsd = pthread_getspecific(rl_key)) == NULL) //本线程第一次调用 recvline
    {
        tsd = Calloc(1, sizeof(Rline));
        pthread_setspecific(rl_key, tsd);
    }
    ptr = vptr;
    for (n = 1; n < maxlen; n++)
    {
        if ((rc = my_recv(tsd, fd,&ch)) == 1)
        {
            *ptr++ = c;
            if (c == '\n')
                break;
        }
        else if (rc == 0)
        {
            *ptr = 0;
            return (n - 1);
        }
        else
            return (-1);
    }
    *ptr = 0;
    return (n);
}
