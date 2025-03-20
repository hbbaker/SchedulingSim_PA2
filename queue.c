#include <stdlib.h>
#include "process.h"

#define MAX_SIZE 4

typedef struct
{
    int size;
    int front;
    int rear;
    Process *processes[MAX_SIZE];
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

int enqueue(Queue *queue, Process *pid)
{
    Process *newProcess = malloc(sizeof(Process));
    if (!newProcess)
    {
        free(newProcess);
        return -1;
    }
    if (isFull(queue))
    {
        free(newProcess);
        return -1;
    }

    if (isEmpty(queue))
    {
        queue->front = 0;
    }

    queue->rear = (queue->rear + 1) % MAX_SIZE;
    queue->processes[queue->rear] = deep_copy(pid);
    queue->size++;

    return 0;
}

Process *dequeue(Queue *queue)
{
    if (isEmpty(queue))
    {
        return NULL;
    }

    Process *dequeuedValue = queue->processes[queue->front];
    queue->front = (queue->front + 1) % MAX_SIZE;
    queue->size--;

    if (isEmpty(queue))
    {
        queue->front = -1;
        queue->rear = -1;
    }

    return dequeuedValue;
}

void free_queue(Queue *queue)
{
    while (queue->size > 0)
    {
        Process *removed = dequeue(queue);
        free(removed);
    }
}
