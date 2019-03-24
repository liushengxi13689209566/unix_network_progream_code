/*************************************************************************
	> File Name: main.cpp
	> Author: Liu Shengxi 
	> Mail: 13689209566@163.com
	> Created Time: 2019年01月06日 星期日 17时08分09秒
 ************************************************************************/

#include <iostream>
using namespace std;
#include <sys/timerfd.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h> /* Definition of uint64_t */
#define handle_error(msg)   \
    do                      \
    {                       \
        perror(msg);        \
        exit(EXIT_FAILURE); \
    } while (0)

static void fun(void)
{
    /*
    struct timespec
    {
    __time_t tv_sec;		//秒
    __syscall_slong_t tv_nsec;	／／纳秒
    };
    */
    static struct timespec start;
    struct timespec curr;
    static int first_call = 1;
    int secs, nsecs;
    if (first_call)
    {
        first_call = 0;
        if (clock_gettime(CLOCK_MONOTONIC, &start) == -1)
            handle_error("clock_gettime");
    }
    if (clock_gettime(CLOCK_MONOTONIC, &curr) == -1)
        handle_error("clock_gettime");

    secs = curr.tv_sec - start.tv_sec;
    nsecs = curr.tv_nsec - start.tv_nsec;
    if (nsecs < 0)
    {
        secs--;
        nsecs += 1000000000;
    }
    printf("%d.%03d: ", secs, (nsecs + 500000) / 1000000);
}

int main(int argc, char *argv[])
{
    /*
        struct itimerspec
      {
        struct timespec it_interval;
        struct timespec it_value; 
        
        it_interval > 0，周期性的时间。
        it_value 是首次超时时间，需要填写从clock_gettime获取的时间，并加上要超时的时间。
        it_value和it_interval都为0表示停止定时器。
      };

    */
    struct itimerspec new_value;
    int max_exp, fd;
    struct timespec now;
    uint64_t exp, tot_exp;
    ssize_t s;
    if ((argc != 2) && (argc != 4))
    {
        fprintf(stderr, "%s init-secs [interval-secs max-exp]\n",
                argv[0]);
        exit(EXIT_FAILURE);
    }
    /*
    clock_gettime比gettimeofday更加精确
    clock_gettime( )提供了纳秒的精确度，给程序计时可是不错哦； 
     */
    if (clock_gettime(CLOCK_REALTIME, &now) == -1)
        handle_error("clock_gettime");

    new_value.it_value.tv_sec = now.tv_sec + atoi(argv[1]);
    new_value.it_value.tv_nsec = now.tv_nsec;
    if (argc == 2)
    {
        new_value.it_interval.tv_sec = 0; //表示只超时一次
        max_exp = 1;
    }
    else
    {
        new_value.it_interval.tv_sec = atoi(argv[2]);
        max_exp = atoi(argv[3]);
    }
    new_value.it_interval.tv_nsec = 0;

    fd = timerfd_create(CLOCK_REALTIME, 0);
    timerfd_settime(fd, TFD_TIMER_ABSTIME, &new_value, NULL);

    fun();
    printf("timer started\n");

    for (tot_exp = 0; tot_exp < max_exp;)
    {
        s = read(fd, &exp, sizeof(uint64_t));
        if (s != sizeof(uint64_t))
            handle_error("read");
        tot_exp += exp;
        fun();
        printf("read: %llu; total=%llu\n",
               (unsigned long long)exp,
               (unsigned long long)tot_exp);
    }
    exit(EXIT_SUCCESS);
}
