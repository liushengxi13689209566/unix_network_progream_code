/*************************************************************************
	> File Name: b.c
	> Author: Liu Shengxi 
	> Mail: 13689209566@163.com
	> Created Time: 2019年03月03日 星期日 21时05分17秒
 ************************************************************************/

#include <stdio.h>

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
    int fd = open("/tmp/example.shm", O_RDWR, 0666);
    if (fd == -1)
    {
        err("open");
    }
    //映射同样一个文件即可　
    int *p = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (p == MAP_FAILED)
    {
        err("mmap");
    }

    close(fd);

    for (;; sleep(1))
    {
        printf("%d\n", *p);
    }

    if (munmap(p, sizeof(int)) == -1)
    {
        err("munmap");
    }

    return EXIT_SUCCESS;
}
