#include "queue.h"

void TSQueueInit(TSQueue *q, QueueType qt)
{
    memset(q, 0, sizeof(TSQueue));
    q->init = 1;
    q->qt = qt;
    pthread_cond_init(&q->cond, NULL);
    pthread_mutex_init(&q->mutex, NULL);
}

void TSQueueEnqueue(TSQueue *q, TSQType e)
{
    qNode * temp;
    pthread_mutex_lock(&q->mutex);
    if (q->qt == FIFO)
    {
        if (q->tail != NULL)
        {
            temp = (qNode *)calloc(1, sizeof(qNode));
            memcpy(&temp->data, &e, sizeof(TSQType));
            q->tail->next = temp;
            q->tail = temp;
        }
        else
        {
            q->tail = (qNode *)calloc(1, sizeof(qNode));
            memcpy(&q->tail->data, &e, sizeof(TSQType));
            q->head = q->tail;
        }
    }
    else
    {
        if (q->head != NULL)
        {
            temp = (qNode *)calloc(1, sizeof(qNode));
            memcpy(&temp->data, &e, sizeof(TSQType));
            temp->next = q->head;
            q->head = temp;
        }
        else
        {
            q->head = (qNode *)calloc(1, sizeof(qNode));
            memcpy(&q->head->data, &e, sizeof(TSQType));
            q->tail = q->head;
        }
    }
    ++(q->count);
    pthread_cond_signal(&q->cond);
    pthread_mutex_unlock(&q->mutex);
}

int TSQueueDequeue(TSQueue *q, TSQType *out)
{
    qNode * temp;

    pthread_mutex_lock(&q->mutex);

    while (q->count == 0)
        pthread_cond_wait(&q->cond, &q->mutex);

    if (q->count == 0)
    {
        pthread_cond_signal(&q->cond);
        pthread_mutex_unlock(&q->mutex);
        return 0;
    }


    memcpy(out, &q->head->data, sizeof(TSQType));

    if (q->head->next)
    {
        temp = q->head;
        q->head = temp->next;
        free(temp);
    }
    else
    {
        free(q->head);
        q->head = NULL;
        q->tail = NULL;
    }

    --q->count;
    pthread_cond_signal(&q->cond);
    pthread_mutex_unlock(&q->mutex);
    return 1;
}

int TSQueueDequeueBlocking(TSQueue *q, TSQType *out)
{
    while(TSQueueDequeue(q, out) == 0);
    return 1;
}

int TSQueueIsEmpty(TSQueue *q)
{
    int t = 0;
    if (q == NULL || q->init == 0)
        return 0;

    pthread_mutex_lock(&q->mutex);
    t = (q->count) ? 0 : 1;
    pthread_mutex_unlock(&q->mutex);
    return t;
}

int TSQueueIsEmptyBlocking(TSQueue *q)
{
    int t = 0;
    if (q == NULL)
        return 0;

    pthread_mutex_lock(&q->mutex);

    if (q->count == 0)
        pthread_cond_wait(&q->cond, &q->mutex);

    t = (q->count) ? 0 : 1;
    pthread_mutex_unlock(&q->mutex);
    return t;
}

void TSQueueDestroy(TSQueue *q)
{
    qNode * temp;
    pthread_mutex_lock(&q->mutex);
    while (q->count--)
    {
        temp = q->head;
        q->head = temp->next;
        free(temp);
    }
    free(q->head);

    q->init = 0;
    q->head = NULL;
    q->tail = NULL;
    pthread_cond_destroy(&q->cond);
    pthread_mutex_unlock(&q->mutex);
    pthread_mutex_destroy(&q->mutex);
}

void TSQueueSig(TSQueue *q)
{
    if (q != NULL && q->init == 1)
        pthread_cond_signal(&q->cond);
}

void TSQueueWaitEmpty(TSQueue *q)
{
    while (1)
    {
        pthread_mutex_lock(&q->mutex);
        while (q->count != 0)
            pthread_cond_wait(&q->cond, &q->mutex);
        pthread_mutex_unlock(&q->mutex);
        break;
    }
}

void TSQueueTrace(TSQueue *q)
{
    pthread_mutex_lock(&q->mutex);
    qNode * temp = q->head;
    if (q->count)
    {
        do
        {
            printf("%d ", temp->data);
            temp = temp->next;
        }
        while (temp);
        printf("\n");
    }
    pthread_mutex_unlock(&q->mutex);
}
