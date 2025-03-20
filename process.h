#ifndef PROCESS_H
#define PROCESS_H

#include <stdlib.h>
#include <sys/time.h>

typedef struct
{
    pid_t pid;
    struct timeval p_start;
    struct timeval p_end;
} Process;

void *initProcess(Process *process, int id);

void *p_start(Process *process);

void *p_end(Process *process);

int get_pid(Process *process);

struct timeval get_p_start(Process *process);

struct timeval get_p_end(Process *process);

Process *deep_copy(Process *src);

#endif