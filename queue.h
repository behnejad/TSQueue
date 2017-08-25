#ifdef __cplusplus
extern "C" {
#endif

#ifndef QUEUE_H
#define QUEUE_H

#include <pthread.h>
#include <stdlib.h>

#define TSQType unsigned int

typedef struct {
    TSQType data;
    struct qNode * next;
} qNode;

typedef struct {
    qNode * head;
    qNode * tail;
    unsigned int count;
    pthread_cond_t cond;
    pthread_mutex_t mutex;
    unsigned int init;
} TSQueue;

// FIFO Queue Implemented
void TSQueueInit(TSQueue *q);
void TSQueueDestroy(TSQueue *q);
void TSQueueEnqueue(TSQueue *q, TSQType e);
int TSQueueDequeue(TSQueue *q, TSQType *out);
int TSQueueDequeueBlocking(TSQueue *q, TSQType *out);
int TSQueueIsEmpty(TSQueue *q);
int TSQueueIsEmptyBlocking(TSQueue *q);
void TSQueueSig(TSQueue *q);
void TSQueueWaitEmpty(TSQueue *q);
void TSQueueTrace(TSQueue *q);


#endif // QUEUE_H

#ifdef __cplusplus
}
#endif
