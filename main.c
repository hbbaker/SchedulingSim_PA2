#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/time.h>
#include "queue.h"
#include "process.h"

// #define TIME_QUANTUM 1000 //in microseconds
pid_t pids[4];

int workloads[4] = {100000, 50000, 25000, 10000}; // Experiment workloads for PA2
// int workloads[4] = {1000, 500, 250, 100}; //Sample workloads for debugging
int remaining[4];                          // remaining workloads
struct timeval start_time[4], end_time[4]; // time tracking variables for running average
int TQ_RR, TQ_MLFQ;                        // user input for TIME_QUANTUM for RR and MLFQ in microseconds
Process *processes[4];

// Calculates the prime factorization of numbers
void myfunction(int param)
{
    int i = 2;
    int j, k;
    while (i < param)
    {
        k = i;
        for (j = 2; j <= k; j++)
        {
            if (k % j == 0)
            {
                k = k / j;
                j--;
                if (k == 1)
                {
                    break;
                }
            }
        }
        i++;
    }
}

// Creates four child processes per workload
void create_processes()
{
    for (int i = 0; i < 4; i++)
    {
        if ((pids[i] = fork()) == 0)
        {
            kill(getpid(), SIGSTOP);
            myfunction(workloads[i]); // Pass process_id
            exit(0);
        }
    }

    for (int i = 0; i < 4; i++)
    {
        initProcess(processes[i], pids[i]);
        printf("PID%d: %d\n", i, processes[i]->pid);
    }
}

// Calculates the response time in microseconds
long long calculateResponseTime(struct timeval start, struct timeval end)
{
    return (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);
}

// Calculates the average response time
void AverageResponseTime(const char *algorithmName)
{
    long long total_time = 0;
    for (int i = 0; i < 4; i++)
    {
        total_time += calculateResponseTime(start_time[i], end_time[i]);
    }
    printf("%s - Average Response Time: %lld us\n\n", algorithmName, total_time / 4);
}

// Function to execute Round Robin Scheduling
void RR_Scheduling()
{
    int num_processes = 4;
    for (int i = 0; i < 4; i++)
    {
        gettimeofday(&start_time[i], NULL);
    }

    int running1 = 1;
    int running2 = 1;
    int running3 = 1;
    int running4 = 1;
    while (running1 > 0 || running2 > 0 || running3 > 0 || running4 > 0)
    {
        if (running1 > 0)
        {
            kill(pids[0], SIGCONT);
            usleep(TQ_RR);
            kill(pids[0], SIGSTOP);
        }
        if (running2 > 0)
        {
            kill(pids[1], SIGCONT);
            usleep(TQ_RR);
            kill(pids[1], SIGSTOP);
        }
        if (running3 > 0)
        {
            kill(pids[2], SIGCONT);
            usleep(TQ_RR);
            kill(pids[2], SIGSTOP);
        }
        if (running4 > 0)
        {
            kill(pids[3], SIGCONT);
            usleep(TQ_RR);
            kill(pids[3], SIGSTOP);
        }
        waitpid(pids[0], &running1, WNOHANG);
        if (running1 == 0)
        {
            gettimeofday(&end_time[0], NULL);
        }
        waitpid(pids[1], &running2, WNOHANG);
        if (running2 == 0)
        {
            gettimeofday(&end_time[1], NULL);
        }
        waitpid(pids[2], &running3, WNOHANG);
        if (running3 == 0)
        {
            gettimeofday(&end_time[2], NULL);
        }
        waitpid(pids[3], &running4, WNOHANG);
        if (running4 == 0)
        {
            gettimeofday(&end_time[3], NULL);
        }
    }
    AverageResponseTime("Round Robin");
}

// Function to compute Shortest Job First Scheduling Algorithm
void SJF_Scheduling()
{
    int done = 0;
    for (int i = 0; i < 4; i++)
    {
        gettimeofday(&start_time[i], NULL);
    }
    while (done < 4)
    {
        int min = -1;
        for (int i = 0; i < 4; i++)
        {
            if (remaining[i] > 0 && (min == -1 || remaining[i] < remaining[min]))
            {
                min = i;
            }
        }
        if (min != -1)
        {
            kill(pids[min], SIGCONT);
            waitpid(pids[min], NULL, 0);
            remaining[min] = 0;
            done++;
            gettimeofday(&end_time[min], NULL);
            /*printf("Process %d completed (SJF)\n", min); //print completion order.*/
        }
    }
    AverageResponseTime("SJF");
}

// Function to compute First Come First Serve Scheduling Algorithm
void FCFS_Scheduling()
{
    for (int i = 0; i < 4; i++)
    {
        gettimeofday(&start_time[i], NULL);
    }
    for (int i = 0; i < 4; i++)
    {
        kill(pids[i], SIGCONT);
        waitpid(pids[i], NULL, 0);
        gettimeofday(&end_time[i], NULL);
    }
    AverageResponseTime("FCFS");
}

// Function to compute Multi-Level Feedback Queue Scheduling Algorithm
void MLFQ_Scheduling()
{
    int num_processes = 4;
    int tq = TQ_MLFQ; // Time Quantum for MLFQ RR in L1
    Process active_process;
    Queue RR_queue;
    Queue FCFS_queue;

    for (int i = 0; i < num_processes; i++)
    {
        enqueue(&RR_queue, processes[i]->pid);
    }

    while (num_processes > 0)
    {
        // L1: Round Robin
        while (getSize(&RR_queue) > 0)
        {
            active_process = *dequeue(&RR_queue);

            gettimeofday(active_process.p_start, NULL); // Start Timer

            // Run for Fixed TQ
            kill(active_process.pid, SIGCONT);
            usleep(tq);
            kill(active_process.pid, SIGSTOP);

            int running;

            // Check if Process is Completed
            waitpid(active_process.pid, &running, WNOHANG);

            // If Complete, set the end time for that process and lower num_processes
            if (running == 0)
            {
                num_processes--;
                gettimeofday(active_process.p_end, NULL);
            }
            else
            {
                enqueue(&FCFS_queue, &active_process);
            }
        }

        // L2: First Come, First Serve
        // Copy of FCFS code from above
        while (getSize(&FCFS_queue) > 0)
        {
            int activePID = dequeue(&FCFS_queue);

            kill(activePID, SIGCONT);
            waitpid(activePID, NULL, 0);
            gettimeofday(&end_time[i], NULL);
            num_processes--;
        }
    }
    AverageResponseTime("MLFQ");
}

int main()
{
    // Prompts user to input Time Quantums for RR Scheduling
    printf("Enter Time Quantum for Round Robin Scheduling: \n");
    scanf("%d", &TQ_RR);
    // Prompts user to input Time Quantums for MLFQ Scheduling
    printf("Enter Time Quantum for MLFQ Scheduling: \n");
    scanf("%d", &TQ_MLFQ);
    create_processes();

    // RR
    for (int i = 0; i < 4; i++)
        remaining[i] = workloads[i];
    printf("Running Round Robin\n");
    RR_Scheduling();

    printf("Press Enter to continue to SJF...\n");
    getchar();
    while (getchar() != '\n')
        ;

    // SJF
    for (int i = 0; i < 4; i++)
        remaining[i] = workloads[i];
    printf("Running SJF\n");
    SJF_Scheduling();

    printf("Press Enter to continue to FCFS...\n");
    getchar();
    while (getchar() != '\n')
        ;

    // FCFS
    for (int i = 0; i < 4; i++)
        remaining[i] = workloads[i];
    printf("Running FCFS\n");
    FCFS_Scheduling();

    printf("Press Enter to continue to MLFQ...\n");
    getchar();
    while (getchar() != '\n')
        ;

    // MLFQ
    for (int i = 0; i < 4; i++)
        remaining[i] = workloads[i];
    printf("Running MLFQ\n");
    MLFQ_Scheduling();
    return 0;
}
