/*************************************************************************
	> File Name: a.c
	> Author: Liu Shengxi 
	> Mail: 13689209566@163.com
	> Created Time: 2019年03月03日 星期日 21时05分11秒
 ************************************************************************/

#include<stdio.h>

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
    // (1) 使用open打开指定的内存映射文件
    int fd = open("/tmp/example.shm", O_RDWR | O_CREAT, 0666);
    if (fd == -1)
    {
        err("open");
    }

    // (2) 文件可能之前不存在，没有页面，此时直接使用将引发一个bus error
    //     先将其截断为指定大小，然后才可以进行相关读取
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

    // (6) 解除内存映射
    if (munmap(p, sizeof(int)) == -1)
    {
        err("munmap");
    }

    return EXIT_SUCCESS;
}