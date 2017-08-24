#ifdef __cplusplus
extern "C" {
#endif

#ifndef QUEUE_H
#define QUEUE_H

#include <pthread.h>
#include <stdlib.h>

#define TSQType int

typedef struct {
    TSQType data;
    struct qNode * previous;
} qNode;

typedef struct {
    qNode * head;
    qNode * tail;
    unsigned int count;
    pthread_cond_t cond;
    pthread_mutex_t mutex;
    int init;
    int mem;
    int deq;
} TSQueue;

// if mem == 1: it will generate a ring queue and donot free memory.
void TSQueueInit(TSQueue *q, int mem);
void TSQueueDestroy(TSQueue *q);
void TSQueueEnqueue(TSQueue *q, TSQType e);
int TSQueueDeQueue(TSQueue *q, TSQType *out);
int TSQueueDeQueueBlocking(TSQueue *q, TSQType *out);
int TSQueueIsEmpty(TSQueue *q);
int TSQueueIsEmptyBlocking(TSQueue *q);
void TSQueueSig(TSQueue *q);
void TSQueueWaitEmpty(TSQueue *q);


#endif // QUEUE_H

#ifdef __cplusplus
}
#endif
