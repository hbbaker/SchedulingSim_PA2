#include <stdlib.h>

typedef struct
{
    int pid;
    int p_start;
    int p_end;
} Process;

void initProcess(Process *process, int pid)
{
    process->pid = pid;
    process->p_start = -1;
    process->p_end = -1;
}

void set_p_start(Process *process, int start_time)
{
    process->p_start = start_time;
}

void set_p_end(Process *process, int end_time)
{
    process->p_end = end_time;
}

int get_pid(Process *process)
{
    return process->pid;
}

int get_p_start(Process *process)
{
    return process->p_start;
}

int get_p_end(Process *process)
{
    return process->p_end;
}