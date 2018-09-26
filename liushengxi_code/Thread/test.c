#include <stdlib.h>
#include <stdio.h>

int main()
{
    int i;
    for (i = 0; i < N; i++)
    {
        pthread_create(&tid, NULL, handle, &i);
    }
}
