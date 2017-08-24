#include "queue.h"

void TSQueueInit(TSQueue *q, int mem)
{
    memset(q, 0, sizeof(TSQueue));
    q->init = 1;
    q->mem = mem;
    pthread_cond_init(&q->cond, NULL);
    pthread_mutex_init(&q->mutex, NULL);
}

void TSQueueEnqueue(TSQueue *q, TSQType e)
{
    qNode * temp;
    pthread_mutex_lock(&q->mutex);

    if (q->head != NULL)
    {
        if (q->mem)
        {
            if (q->head->previous == q->head || q->head->previous == q->tail)
            {
                temp = (qNode *)calloc(1, sizeof(qNode));
                temp->data = e;
                temp->previous = q->tail;
                q->head->previous = temp;
                q->head = temp;
            }
            else
            {
                q->head = q->head->previous;
                q->head->data = e;
            }
        }
        else
        {
            temp = (qNode *)calloc(1, sizeof(qNode));
            temp->data = e;
            q->head->previous = temp;
            q->head = temp;
        }
    }
    else
    {
        q->head = (qNode *)calloc(1, sizeof(qNode));
        q->head->data = e;
        q->head->previous = q->head;
        q->tail = q->head;
    }
    ++(q->count);
    pthread_cond_signal(&q->cond);
    pthread_mutex_unlock(&q->mutex);
}

int TSQueueDeQueue(TSQueue *q, TSQType *out)
{
    qNode * temp;

    pthread_mutex_lock(&q->mutex);

    if (q->count == 0)
        pthread_cond_wait(&q->cond, &q->mutex);

    if (q->count == 0)
    {
        pthread_cond_signal(&q->cond);
        pthread_mutex_unlock(&q->mutex);
        return 0;
    }

    *out = q->tail->data;
    if (q->mem)
    {
       q->tail = q->tail->previous;
    }
    else
    {
        if (q->tail->previous)
        {
            temp = q->tail;
            q->tail = temp->previous;
            free(temp);
        }
        else
        {
            free(q->tail);
            q->head = NULL;
            q->tail = NULL;
        }
    }

    --q->count;
    ++q->deq;
//    printf("{%d, %d, %d}\n", q->deq, q->count, q->deq + q->count);
    pthread_cond_signal(&q->cond);
    pthread_mutex_unlock(&q->mutex);
    return 1;
}

int TSQueueDeQueueBlocking(TSQueue *q, TSQType *out)
{
    while(TSQueueDeQueue(q, out) == 0);
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

    while (q->count)
    {
        temp = q->tail;
        q->tail = temp->previous;
        free(temp);
        --(q->count);
    }
    q->init = 0;
    pthread_mutex_unlock(&q->mutex);
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
