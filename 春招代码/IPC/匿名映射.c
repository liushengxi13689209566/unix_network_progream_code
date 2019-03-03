// 注释部分代码为使用/dev/zero的情况
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>

void err(const char *str)
{
    perror(str);
    exit(EXIT_FAILURE);
}

int main(int argc, const char *argv[])
{
    int *p = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0);
    // 或者　MAP_SHARED | MAP_ANONYMOUS

    if (p == MAP_FAILED)
    {
        err("mmap");
    }

    // close(fd);

    pid_t id = fork();
    //子进程
    if (id == 0)
    {
        for (;; sleep(1))
        {
            printf("%d\n", *p);
        }
    }
    //pid > 0
    else
    {
        for (srand(7777);; sleep(1))
        {
            int v = rand();
            *p = v;
            printf("produce: %d\n", v);
        }
    }

    if (munmap(p, sizeof(int)) == -1)
    {
        err("munmap");
    }

    return EXIT_SUCCESS;
}
