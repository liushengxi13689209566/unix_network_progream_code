/*************************************************************************
	> File Name: testTSD.c
	> Author: Liu Shengxi 
	> Mail: 13689209566@163.com
	> Created Time: 2018年09月27日 星期四 22时11分48秒
 ************************************************************************/
#include "../myhead.h"

static pthread_key_t global_key;
static pthread_once_t control_once = PTHREAD_ONCE_INIT; // 0
// 用来销毁每个线程所指向的实际数据
static void pth_destructor(void *ptr)
{
    printf("销毁线程。。。。。\n");
    free(ptr);
}
static void pro_once(void)
{
    printf("\t获取进程键\n");
    pthread_key_create(&global_key, pth_destructor); //第二个参数是线程释放函数
}
typedef struct
{
    int id;
    char *str;
} TT;
void *handle(void *arg)
{
    TT *tsd;
    char *tmp = ((TT *)arg)->str;
    int i = ((TT *)arg)->id;

    free(arg);

    pthread_once(&control_once, pro_once); //在进程范围内只被调用一次，其实就是取得 key[n] 和 pkey[n]的索引

    if ((tsd = pthread_getspecific(global_key)) == NULL) //测试在当前这个线程中有没有分配线程的实际数据
    {
        tsd = (TT *)malloc(sizeof(TT));
        tsd->id = i;
        tsd->str = tmp;
        pthread_setspecific(global_key, tsd); //存放线程特定数据
        printf("%d pthread setspecific, address: %p\n", i, tsd);
    }
    /*获取*/
    TT *temp = pthread_getspecific(global_key);
    printf("temp->id ==  %d ,temp->str == %s \n", temp->id, temp->str);
    sleep(5);

    /*再次获取*/
    temp = pthread_getspecific(global_key);
    printf("temp->id ==  %d ,temp->str == %s \n", temp->id, temp->str);
    
    pthread_exit(NULL);
}
int main()
{
    pthread_t tid1, tid2;
    int i;
    TT *iptr;
    for (i = 1; i < 3; i++)
    {
        iptr = (TT *)malloc(sizeof(TT));
        (*iptr).id = i;
        i == 1 ? ((*iptr).str = "11111111111") : ((*iptr).str = "222222222222222222222");
        pthread_create(&tid1, NULL, handle, iptr);
    }

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);

    pthread_key_delete(global_key);

    return 0;
}
