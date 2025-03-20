#ifndef PROCESS_H
#define PROCESS_H

#include <stdlib.h>

typedef struct
{
    int pid;
    int p_start;
    int p_end;
} Process;

void *initProcess(Process *process, int pid);

void *set_p_start(Process *process, int start_time);

void *set_p_end(Process *process, int end_time);

int get_pid(Process *process);

int get_p_start(Process *process);

int get_p_end(Process *process);

#endif