
#include "../myhead.h"
#define MAX 10
/*利用条件变量实现 thr_join 等待任一线程结束 */
int ndone = 0;
pthread_mutex_t ndone_mutex;
pthread_cond_t ndone_cond;
int data[MAX];
int Decide = MAX;

void *handle(void *arg)
{
    sleep(5);

    pthread_mutex_lock(&ndone_mutex);
    printf("我要推出了哦！！我是 %d 号线程哦 ！！！\n", pthread_self());
    ndone++;
    *((int *)arg) = 1;
    Decide--;
    pthread_cond_signal(&ndone_cond);
    pthread_mutex_unlock(&ndone_mutex);
    return 0;
}
int main(void)
{
    pthread_t tid[MAX];
    int i;
    Decide = MAX;
    for (i = 0; i < MAX; i++)
    {
        data[i] = 0;
        pthread_create(&tid[i], NULL, handle, &data[i]);
    }
    while (Decide > 0)
    {
        pthread_mutex_lock(&ndone_mutex);
        while (ndone == 0) //说明没有任何线程退出
        {
            pthread_cond_wait(&ndone_cond, &ndone_mutex);
        }
        //一定有某个线程退出 
        for (i = 0; i < MAX; i++)
        {
            if (data[i] == 1){
                printf("pthread_join 等待 %d 线程退出 \n",tid[i]);
                pthread_join(tid[i], NULL);
                data[i] = 0 ;
            }
        }
        pthread_mutex_unlock(&ndone_mutex);
    }
    return 0;
}
