#include "queue.h"

void TSQueueInit(TSQueue *q, QueueType qt, int mem)
{
    memset(q, 0, sizeof(TSQueue));
    q->init = 1;
    q->mem = mem;
    q->qt = qt;
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
//                temp->data = e;
                memcpy(&temp->data, &e, sizeof(TSQType));
                temp->previous = q->tail;
                q->tail->next = temp;
                q->head->previous = temp;
                temp->next = q->head;
                q->head = temp;
            }
            else
            {
                q->head = q->head->previous;
//                q->head->data = e;
                memcpy(&q->head->data, &e, sizeof(TSQType));
            }
        }
        else
        {
            temp = (qNode *)calloc(1, sizeof(qNode));
//            temp->data = e;
            memcpy(&temp->data, &e, sizeof(TSQType));
            q->head->previous = temp;
            temp->next = q->head;
            q->head = temp;
        }
    }
    else
    {
        q->head = (qNode *)calloc(1, sizeof(qNode));
//        q->head->data = e;
        memcpy(&q->head->data, &e, sizeof(TSQType));
        q->head->previous = q->head;
        q->head->next = q->head;
        q->tail = q->head;
    }
    ++(q->count);
    pthread_cond_signal(&q->cond);
    pthread_mutex_unlock(&q->mutex);
}

int TSQueueDequeue(TSQueue *q, TSQType *out)
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

//    *out = q->tail->data;
    if (q->mem)
    {
        if (q->qt == FIFO)
        {
            memcpy(out, &q->tail->data, sizeof(TSQType));
            q->tail = q->tail->previous;
        }
        else
        {
            memcpy(out, &q->head->data, sizeof(TSQType));
            q->head = q->head->next;
        }
    }
    else
    {
        if (q->qt == FIFO)
        {
            memcpy(out, &q->tail->data, sizeof(TSQType));
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
        else
        {
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
        }
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
