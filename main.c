#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/time.h>

//#define TIME_QUANTUM 1000 //in microseconds
pid_t pids[4];
int workloads[4] = {100000, 50000, 25000, 10000}; //Experiment workloads for PA2
//int workloads[4] = {1000, 500, 250, 100}; //Sample workloads for debugging
int remaining[4]; //remaining workloads
struct timeval start_time[4], end_time[4]; //time tracking variables for running average
int TQ_RR, TQ_MLFQ; //user input for TIME_QUANTUM for RR and MLFQ in microseconds

//Calculates the prime factorization of numbers
void myfunction(int param, int process_id) {
    int num = param, factor = 2;
    /*printf("Process %d: Prime factors of %d: ", process_id, param);*/ //Prints the factorization of each process
    while (num > 1) {
        while (num % factor == 0) {
            /*printf("%d ", factor);*/ //Prints the factorization of each process
            num /= factor;
        }
        factor++;
    }
    fflush(stdout); //forces the output
}

//Creates four child processes per workload
void create_processes() {
    for (int i = 0; i < 4; i++) {
        if ((pids[i] = fork()) == 0) {
            kill(getpid(), SIGSTOP);
            myfunction(workloads[i], i); // Pass process_id
            exit(0);
        }
    }
}

//Calculates the response time in microseconds
long long calculateResponseTime(struct timeval start, struct timeval end) {
    return (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);
}

//Calculates the average response time 
void AverageResponseTime(const char *algorithmName) {
    long long total_time = 0;
    for (int i = 0; i < 4; i++) {
        total_time += calculateResponseTime(start_time[i], end_time[i]);
    }
    printf("%s - Average Response Time: %lld us\n\n", algorithmName, total_time / 4);
}

//Function to execute Round Robin Scheduling
void RR_Scheduling() {
    int num_processes = 4;
    for (int i = 0; i < 4; i++) {
        gettimeofday(&start_time[i], NULL);
    }
    while (num_processes > 0) {
        for (int i = 0; i < 4; i++) {
            if (remaining[i] > 0) {
                kill(pids[i], SIGCONT);
                int slice = (remaining[i] < TQ_RR) ? remaining[i] : TQ_RR;
                usleep(slice);
                kill(pids[i], SIGSTOP);
                remaining[i] -= slice;
                if (remaining[i] <= 0) {
                    num_processes--;
                    gettimeofday(&end_time[i], NULL);
                    waitpid(pids[i], NULL, 0); //wait for process to completely finish.
                }
            }
        }
    }
    AverageResponseTime("Round Robin");
}

// Function to compute Shortest Job First Scheduling Algorithm
void SJF_Scheduling() {
    int done = 0;
    for (int i = 0; i < 4; i++) {
        gettimeofday(&start_time[i], NULL);
    }
    while (done < 4) {
        int min = -1;
        for (int i = 0; i < 4; i++) {
            if (remaining[i] > 0 && (min == -1 || remaining[i] < remaining[min])) {
                min = i;
            }
        }
        if (min != -1) {
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
void FCFS_Scheduling() {
    for (int i = 0; i < 4; i++) {
        gettimeofday(&start_time[i], NULL);
    }
    for (int i = 0; i < 4; i++) {
        kill(pids[i], SIGCONT);
        waitpid(pids[i], NULL, 0);
        gettimeofday(&end_time[i], NULL);
    }
    AverageResponseTime("FCFS");
}

// Function to compute Multi-Level Feedback Queue Scheduling Algorithm
void MLFQ_Scheduling() {
    int num_processes = 4;
    int queue[4] = {0, 0, 0, 0}; // Track which queue each process is in
    int tq = TQ_MLFQ; // Time Quantum for MLFQ RR in L1
    int queue_level = 0; // Number of processes in L2

    for (int i = 0; i < 4; i++) {
        gettimeofday(&start_time[i], NULL);
    }

    while (num_processes > 0) {
        // L1: Round Robin
        for (int i = 0; i < 4; i++) {
            if (remaining[i] > 0 && queue[i] == 0) {
                kill(pids[i], SIGCONT);
                int rem = (remaining[i] < tq) ? remaining[i] : tq;
                usleep(rem);
                kill(pids[i], SIGSTOP);
                remaining[i] -= rem;

                if (remaining[i] <= 0) {
                    num_processes--;
                    gettimeofday(&end_time[i], NULL);
                    waitpid(pids[i], NULL, 0);
                } else {
                    queue[i] = 1; // Move to Level 2
                    queue_level++; //Increments the queue level
                }
            }
        }

        // L2: First Come, First Serve
        // Copy of FCFS code from above
        for (int i = 0; i < 4; i++) {
            if (remaining[i] > 0 && queue[i] == 1) {
                kill(pids[i], SIGCONT);
                waitpid(pids[i], NULL, 0);
                gettimeofday(&end_time[i], NULL);
                remaining[i] = 0;
                num_processes--;
                queue_level--;
            }
        }
    }
    AverageResponseTime("MLFQ");
}


int main() {
    //Prompts user to input Time Quantums for RR Scheduling
    printf("Enter Time Quantum for Round Robin Scheduling: \n");
    scanf("%d", &TQ_RR);
    //Prompts user to input Time Quantums for MLFQ Scheduling
    printf("Enter Time Quantum for MLFQ Scheduling: \n");
    scanf("%d", &TQ_MLFQ);
    create_processes();

    //RR
    for (int i = 0; i < 4; i++) remaining[i] = workloads[i];
    printf("Running Round Robin\n");
    RR_Scheduling();
    
    printf("Press Enter to continue to SJF...\n");
    getchar();
    while (getchar() != '\n');
    
    //SJF
    for (int i = 0; i < 4; i++) remaining[i] = workloads[i];
    printf("Running SJF\n");
    SJF_Scheduling();
    
    printf("Press Enter to continue to FCFS...\n");
    getchar();
    while (getchar() != '\n');
     
    //FCFS
    for (int i = 0; i < 4; i++) remaining[i] = workloads[i];
    printf("Running FCFS\n");
    FCFS_Scheduling();
    
    printf("Press Enter to continue to MLFQ...\n");
    getchar();
    while (getchar() != '\n');
    
    //MLFQ
    for (int i = 0; i < 4; i++) remaining[i] = workloads[i];
    printf("Running MLFQ\n");
    MLFQ_Scheduling();
    return 0;
}
