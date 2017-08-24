#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#include "queue.h"

TSQueue queue;

pthread_mutex_t  mutex = PTHREAD_MUTEX_INITIALIZER;

int id = 0;
int c = 0;
int totalSignal = 0;
int uslp = 1000;
int loop = 10000;
int forloop = 1000;
int sig = 0;
#define t 1000000

unsigned long int getTime()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return 1000000 * tv.tv_sec + tv.tv_usec;
}

void * producer(void * id)
{
    int i = 0;
    int ir = (int)id;
//    printf("%lu Thread %d\n", getTime(), ir);
    while(i < loop)
    {
        TSQueueEnqueue(&queue, ir);
        pthread_mutex_lock(&mutex);
        ++totalSignal;
        ++sig;
        pthread_mutex_unlock(&mutex);
//        usleep(uslp);
        ++i;
    }
//    printf("%lu Thread %d finished\n", getTime(), ir);
    return 0;
}

void * consumer(void * id)
{
//    printf("%lu consumer \n", getTime());
    int b;
    while(1)
    {
        if (TSQueueDequeue(&queue, &b))
        {
//            printf("%lu %d\n", getTime(), b);
            pthread_mutex_lock(&mutex);
            --sig;
            ++c;
            pthread_mutex_unlock(&mutex);
        }
        else
            break;
//        usleep(uslp);
    }
//    printf("%lu consumer done\n", getTime());
    return 0;
}

int main(int argc, char *argv[])
{
    pthread_t a[t], con;
    TSQueueInit(&queue, FIFO, 1);

//    for (int i = 0; i < t; ++i)
//    {
//        TSQueueEnqueue(&queue, i);
////        printf("Enqueue %d\n", i);
//    }


//    for (int i = 0; i < t; ++i)
//    {
//        int q;
//        TSQueueDequeue(&queue, &q);
////        printf("Dequeue %d\n", q);
//    }

//    TSQueueDestroy(&queue);
//    return 0;

    for (int l = 0; l < forloop; ++l)
    {
        c = 0;
        totalSignal = 0;
        sig = 0;

        for (int i = 0; i < t; ++i)
            pthread_create(&a[i], NULL, producer, i+1);

        pthread_create(&con, NULL, consumer, NULL);

        for (int i = 0; i < t; ++i)
            pthread_join(a[i], NULL);

        TSQueueWaitEmpty(&queue);
//        printf("E\n");
        usleep(1000);
        TSQueueSig(&queue);

        pthread_join(con, NULL);
        printf("%03d) remains: %d, catched: %d, total: %d\n", l, sig, c, totalSignal);
    }
    TSQueueDestroy(&queue);
    return 0;
}
