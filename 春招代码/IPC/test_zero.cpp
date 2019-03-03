/*************************************************************************
	> File Name: test_zero.cpp
	> Author: Liu Shengxi 
	> Mail: 13689209566@163.com
	> Created Time: 2019年03月03日 星期日 16时33分56秒
 ************************************************************************/

#include <iostream>
using namespace std;
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h> // mmap
#include <unistd.h>

int main(void)
{
    // 记录/dev/zero文件标识符
    int fd;
    // 迭代索引
    int i;
    // 计数器
    int counter;
    // 进程ID
    pid_t pid;
    // 映射区域首地址
    void *area;
    // 打开/dev/zero文件
    if ((fd = open("/dev/zero", O_RDWR)) < 0)
    {
        err_sys("open error");
    }
    // 映射/dev/zero文件
    if ((area = mmap(0, SIZE, PROT_READ | PROT_WRITE,
                     MAP_SHARED, fd, 0)) == MAP_FAILED)
    {
        err_sys("mmap error");
    }
    // 关闭/dev/zero文件
    close(fd);
}
