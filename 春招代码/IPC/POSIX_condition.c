#include <semaphore.h>
#include <sys/mman.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>

void err(const char *str)
{
    perror(str);
    exit(EXIT_FAILURE);
}

// 共享内存区域
struct memory_area
{
    sem_t sem; // 信号量，用于同步和解决竞争
    int num;   // 存储产生的随机数
};

// 生产随机数函数
void produce(struct memory_area *p)
{
    for (srand(p->num);; sleep(1))
    {
        sem_wait(&(p->sem));
        int v = rand();
        p->num = v;
        printf("[%d] produce: %d\n", getpid(), v);
    }
}

// 打印随机数函数
void consume(struct memory_area *p)
{
    for (;; sleep(1))
    {
        printf("consume: %d\n", p->num);
        sem_post(&(p->sem));
    }
}

int main(int argc, const char *argv[])
{
    // 建立共享内存区域
    struct memory_area *p = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE,
                                 MAP_SHARED | MAP_ANON, -1, 0);
    if (p == MAP_FAILED)
        err("mmap");

    // 初始化信号量
    sem_init(&(p->sem), 1, 1);

    // 创建进程并分别执行打印和产生随机数的函数
    pid_t id1, id2;
    if ((id1 = fork()) != 0)
    {
        //父进程
        if ((id2 = fork()) != 0)
        {
            //父进程
            consume(p);
        }
        //子进程
        else
        {
            produce(p);
        }
    }
    //子进程
    else
    {
        produce(p);
    }

    // 销毁信号量
    sem_destroy(&(p->sem));
    // 解除映射
    if (munmap(p, sizeof(int)) == -1)
        err("munmap");

    return EXIT_SUCCESS;
}
