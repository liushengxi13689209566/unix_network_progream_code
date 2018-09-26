/*************************************************************************
	> File Name: example01..c
	> Author: Liu Shengxi 
	> Mail: 13689209566@163.com
	> Created Time: 2018年09月26日 星期三 21时07分00秒
 ************************************************************************/

#include "../myhead.h"

#define NLOOP 5000

int counter; /* incremented by threads */

void *doit(void *);

int main(int argc, char **argv)
{
	pthread_t tidA, tidB;

	pthread_create(&tidA, NULL, &doit, NULL);
	pthread_create(&tidB, NULL, &doit, NULL);

	/* 4wait for both threads to terminate */
	pthread_join(tidA, NULL);
	pthread_join(tidB, NULL);

	printf("counter == %d \n",counter);

	exit(0);
}

void *
doit(void *vptr)
{
	int i, val;

	/*
	 * Each thread fetches, prints, and increments the counter NLOOP times.
	 * The value of the counter should increase monotonically.
	 */

	for (i = 0; i < NLOOP; i++)
	{
		val = counter;
		printf("%d: %d\n", pthread_self(), val + 1);
		counter = val + 1;
	}

	return (NULL);
}

