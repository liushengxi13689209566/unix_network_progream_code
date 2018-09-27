/*************************************************************************
	> File Name: csdn01.c
	> Author: Liu Shengxi 
	> Mail: 13689209566@163.com
	> Created Time: 2018年09月26日 星期三 23时17分03秒
 ************************************************************************/

#include "../myhead.h"
#define MAX 10
void *handle(void *arg)
{
    int i = *((int *)arg);
    free(arg);
    printf("线程：%d , 初始化 i == %d \n", pthread_self(), i);
    sleep(rand() % 10);
    printf("线程：%d , 退出时 i == %d \n", pthread_self(), i);
    return 0;
}
int main(void)
{
    pthread_t tid[MAX];
    int i;
    int *iptr = NULL;
    for (i = 0; i < MAX; i++)
    {
        iptr = (int *)malloc(sizeof(int));
        *iptr = i;
        pthread_create(&tid[i], NULL, handle, iptr);
    }
    for (i = 0; i < MAX; i++)
    {
        pthread_join(tid[i], NULL);
    }
    return 0;
}