// 文件：a.c
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/stat.h> /* For mode constants */

#include <stdio.h>
#include <stdlib.h>

void err(const char *str)
{
    perror(str);
    exit(EXIT_FAILURE);
}

int main(int argc, const char *argv[])
{
    // (1) 使用shm_open创建新的共享内存
    int fd = shm_open("example", O_RDWR | O_CREAT, 0666);
    if (fd == -1)
    {
        err("shm_open");
    }

    // (2) 设定共享内存的大小
    if (ftruncate(fd, sizeof(int)) == -1)
    {
        err("ftruncate");
    }

    // (3) 调用mmap将内存映射文件映射到自身的地址空间中
    int *p = mmap(NULL, sizeof(int), PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);
    if (p == MAP_FAILED)
    {
        err("mmap");
    }

    // (4) 完成映射之后可以关闭fd，不影响已经建立的映射
    close(fd);

    // (5) 产生随机数并存入共享内存中，使用sleep为了便于观察
    for (srand(7777);; sleep(1))
    {
        int v = rand();
        *p = v;
        printf("produce: %d\n", v);
    }

    // (6) 在文件系统中删除共享内存区对象
    if (shm_unlink("example") == -1)
    {
        err("shm_unlink");
    }

    // (7) 解除内存映射
    if (munmap(p, sizeof(int)) == -1)
    {
        err("munmap");
    }

    return EXIT_SUCCESS;
}