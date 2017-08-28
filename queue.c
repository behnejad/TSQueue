#include "queue.h"

void TSQueueInit(TSQueue *q, QueueType qt, int mem)
{
    memset(q, 0, sizeof(TSQueue));
    q->init = 1;
    q->qt = qt;
    q->mem = mem;
    pthread_cond_init(&q->cond, NULL);
    pthread_mutex_init(&q->mutex, NULL);
}

void TSQueueEnqueue(TSQueue *q, TSQType e)
{
    if (q->init == 0)
        return;

    qNode * temp;
    pthread_mutex_lock(&q->mutex);
    /* FIFO */
    if (q->qt == FIFO)
    {
        if (q->tail)
        {
            /* If Tail has a next element */
            if (q->tail->next)
            {
                q->tail = q->tail->next;
                memcpy(&q->tail->data, &e, sizeof(TSQType));
            }
            /* If Tail is the last element */
            else
            {
                temp = (qNode *)calloc(1, sizeof(qNode));
                memcpy(&temp->data, &e, sizeof(TSQType));
                q->tail->next = temp;
                if (q->mem)
                    temp->prev = q->tail;
                q->tail = temp;
            }
        }
        /* Queue is Empty */
        else
        {
            q->tail = (qNode *)calloc(1, sizeof(qNode));
            memcpy(&q->tail->data, &e, sizeof(TSQType));
            q->head = q->tail;
        }
    }
    /* LIFO */
    else
    {
        /* Existing Element in Queue */
        if (q->head)
        {
            /* If Head has a previous element */
            if (q->head->prev)
            {
                q->head = q->head->prev;
                memcpy(&q->head->data, &e, sizeof(TSQType));
            }
            /* If Head is the first element in Queue */
            else
            {
                temp = (qNode *)calloc(1, sizeof(qNode));
                memcpy(&temp->data, &e, sizeof(TSQType));
                temp->next = q->head;
                if (q->mem)
                    q->head->prev = temp;
                q->head = temp;
            }
        }
        /* Queue is Empty */
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

    if (q->init == 0)
        return 0;

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

    /* If Queue has more than 1 elements */
    if (q->head->next)
    {
        /* If we have mem == 1 */
        if (q->mem)
        {
            /* FIFO */
            if (q->qt == FIFO)
            {
                /* Moving first element of Queue to the end of Queue */
                temp = q->head;
                q->head = q->head->next;
                q->head->prev == NULL;
                temp->next == NULL;
                q->tail->next = temp;
                temp->prev = q->tail;
            }
            /* LIFO */
            else
            {
                q->head = q->head->next;
            }
        }
        /* FIFO and LIFO with releasing memory after dequeue */
        else
        {
            temp = q->head;
            q->head = temp->next;
            free(temp);
        }
    }
    /* If Queue has 1 element */
    else
    {
        if (q->mem == 0)
        {
            free(q->head);
            q->head = NULL;
            q->tail = NULL;
        }
    }

    --q->count;
    pthread_cond_signal(&q->cond);
    pthread_mutex_unlock(&q->mutex);
    return 1;
}

int TSQueueDequeueBlocking(TSQueue *q, TSQType *out)
{
    if (q->init == 0)
        return 0;

    while(TSQueueDequeue(q, out) == 0);
    return 1;
}

int TSQueueIsEmpty(TSQueue *q)
{
    if (q->init == 0)
        return 0;

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
    if (q->init == 0)
        return 0;

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
    if (q->init == 0)
        return;

    qNode * temp;
    pthread_mutex_lock(&q->mutex);
    /* If we have some unused elements in Queue with mem == 1 */
    if (q->mem)
    {
        if (q->qt == LIFO)
        {
            /* moving to the head of Queue */
            while (q->head && q->head->prev)
            {
                q->head = q->head->prev;
                ++q->count;
            }
        }
        else
        {
            /* moving to the end of Queue */
            while (q->tail && q->tail->next)
            {
                q->tail = q->tail->next;
                ++q->count;
            }
        }
    }
    /* Releasing Memory from the head of Queue */
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
    if (q->init == 0)
        return;

    if (q != NULL && q->init == 1)
        pthread_cond_signal(&q->cond);
}

void TSQueueWaitEmpty(TSQueue *q)
{
    if (q->init == 0)
        return;

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
    if (q->init == 0)
        return;

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
