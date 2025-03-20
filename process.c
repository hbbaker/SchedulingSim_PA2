#include <stdlib.h>
#include <sys/time.h>
#include <string.h>

typedef struct
{
    pid_t pid;
    struct timeval p_start;
    struct timeval p_end;
} Process;

void initProcess(Process *process, int id)
{
    process->pid = id;
}

void p_start(Process *process)
{
    gettimeofday(&process->p_start, NULL);
}

void p_end(Process *process)
{
    gettimeofday(&process->p_end, NULL);
}

int get_pid(Process *process)
{
    return process->pid;
}

struct timeval get_p_start(Process *process)
{
    return process->p_start;
}

struct timeval get_p_end(Process *process)
{
    return process->p_end;
}

Process *deep_copy(Process *src)
{
    Process *copy = malloc(sizeof(Process));
    copy->pid = src->pid;
    copy->p_start = src->p_start;
    copy->p_end = src->p_end;
    return copy;
}

void freeProcess(Process *proc)
{
    free(proc);
}
