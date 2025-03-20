#include <stdlib.h>

#define MAX_SIZE 4

typedef struct
{
    int size;
    int front;
    int rear;
    int pids[MAX_SIZE];
} Queue;

void initQueue(Queue *queue)
{
    queue->front = -1;
    queue->rear = -1;
    queue->size = 0;
}

int getSize(Queue *queue)
{
    return queue->size;
}

int isFull(Queue *queue)
{
    return queue->size == MAX_SIZE;
}

int isEmpty(Queue *queue)
{
    return queue->size == 0;
}

int enqueue(Queue *queue, int pid)
{
    if (isFull(queue))
    {
        return -1;
    }

    if (isEmpty(queue))
    {
        queue->front = 0;
    }

    queue->rear = (queue->rear + 1) % MAX_SIZE;
    queue->pids[queue->rear] = pid;
    queue->size++;

    return 0;
}

int dequeue(Queue *queue)
{
    if (isEmpty(queue))
    {
        return -1;
    }

    int dequeuedValue = queue->pids[queue->front];
    queue->front = (queue->front + 1) % MAX_SIZE;
    queue->size--;

    if (isEmpty(queue))
    {
        queue->front = -1;
        queue->rear = -1;
    }

    return dequeuedValue;
}