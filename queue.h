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

typedef enum {
    FIFO,
    LIFO
} QueueType;

typedef struct {
    qNode * head;
    qNode * tail;
    unsigned int count;
    pthread_cond_t cond;
    pthread_mutex_t mutex;
    unsigned int init;
    QueueType qt;
} TSQueue;

// FIFO And LIFO Queue
void TSQueueInit(TSQueue *q, QueueType qt);
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
