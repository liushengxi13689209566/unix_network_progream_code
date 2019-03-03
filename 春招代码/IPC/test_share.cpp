/*************************************************************************
	> File Name: test_share.cpp
	> Author: Liu Shengxi 
	> Mail: 13689209566@163.com
	> Created Time: 2019年03月03日 星期日 15时55分14秒
 ************************************************************************/

#include <iostream>
#include <stdio.h>
#include <sys/shm.h>

#define ARRAY_SIZE 40000
#define MALLOC_SIZE 100000
#define SHM_SIZE 100000
#define SHM_MODE 0600 /*r/w*/

char array[ARRAY_SIZE]; /*data =  bss*/

int main(void)
{
    int shmid;    //共享内存标识符
    char *ptr;    //内存分配首地址
    char *shmptr; //关联共享内存首地址
    /*打印数组的起始、结束地址*/
    printf("array[] from %p to %p\n",
           (void *)&array[0],
           (void *)&array[ARRAY_SIZE]);

    //打印当前共享内存标识符地址
    printf("stack around %p\n", (void*)&shmid);
    //分配内存
    if ((ptr = (char *)malloc(MALLOC_SIZE)) == NULL)
    {
        printf("malloc error\n");
    }
    //打印内存起始、结束地址
    printf("malloced from %p to %p\n", (void*)ptr,
           (void*)ptr + MALLOC_SIZE);
    //创建共享内存
    shmid = shmget(IPC_PRIVATE, SHM_SIZE, SHM_MODE);
    if (shmid < 0)
        printf("error\n");

    //关联共享内存
    shmptr = (char *)shmat(shmid, 0, 0);
    if (shmptr == (void *)-1)
        printf("error\n");

    //打印共享内存起始、结束地址
    printf("shared memory attached form %p to %p\n",
           (void*)shmptr, (void*)shmptr + SHM_SIZE);

    free(ptr);
    if (shmctl(shmid, IPC_RMID, 0) < 0)
    {
        printf("shmctl error");
    }
    //退出进程
    exit(0);
}
