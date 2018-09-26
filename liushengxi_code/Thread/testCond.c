
#include "../myhead.h"
#define MAX 10
/*利用条件变量实现 thr_join 等待任一线程结束 */
int ndone = 0;
pthread_mutex_t ndone_mutex;
pthread_cond_t ndone_cond;
int data[MAX];
int nleftthreads = MAX;

void *handle(void *arg)
{
    sleep(rand() % 10);

    int tag = (int)arg;

    pthread_mutex_lock(&ndone_mutex);
    printf("我要推出了哦！！我是 %d 号线程哦 ！！！\n", pthread_self());
    ndone++;
    tag = 1;
    nleftthreads--;
    pthread_cond_signal(&ndone_cond);
    pthread_mutex_unlock(&ndone_mutex);
    return 0;
}
int main(void)
{
    pthread_t tid[MAX];
    int i;
    nleftthreads = MAX;
    for (i = 0; i < MAX; i++)
    {
        data[i] = 0;
        pthread_create(&tid[i], NULL, handle, &data[i]);
    }
    while (nleftthreads > 0)
    {
        pthread_mutex_lock(&ndone_mutex);
        while (ndone == 0) //说明没有任何线程退出
        {
            pthread_cond_wait(&ndone_cond, &ndone_mutex);
        }
        //一定有某个线程退出
        for (i = 0; i < MAX; i++)
        {
            if (data[i] == 1)
                pthread_join(tid[i], NULL);
        }
        pthread_mutex_unlock(&ndone_mutex);
    }
    return 0;
}
