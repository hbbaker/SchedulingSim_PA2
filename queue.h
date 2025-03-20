#ifndef QUEUE_H
#define QUEUE_H

#include <stdlib.h>
#include "process.h"

#define MAX_SIZE 4

typedef struct
{
    int size;
    int front;
    int rear;
    int pids[MAX_SIZE];
} Queue;

void *initQueue(Queue *queue);

int getSize(Queue *queue);

int isFull(Queue *queue);

int isEmpty(Queue *queue);

int enqueue(Queue *queue, Process *pid);

Process *dequeue(Queue *queue);

#endif