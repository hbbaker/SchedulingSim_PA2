#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/time.h>

//#define TIME_QUANTUM 1000 //in microseconds
pid_t pids[4];
//int workloads[4] = {100000, 50000, 25000, 10000}; //Experiment workloads
int workloads[4] = {1000, 500, 250, 100}; //debug sample workloads
int remaining[4]; //remaining workloads
struct timeval start_time[4], end_time[4]; //time tracking variables for average
int TQ_RR, TQ_MLFQ; //user input for TIME_QUANTUM for RR and MLFQ in us

//Calculates the prime factorization of numbers
void myfunction(int param) { //takes the input param variable
    for (int i = 2; i <= param; i++) {
        int num = i, factor = 2;
        while (num > 1) {
            while (num % factor == 0) {
                num /= factor;
            }
            factor++;
        }
    }
}

//Creates four child processes per workload
void create_processes() {
    for (int i = 0; i < 4; i++) {
        if ((pids[i] = fork()) == 0) {
            kill(getpid(), SIGSTOP);
            myfunction(workloads[i]);
            exit(0);
        }
    }
}

//Calculates the response time in microseconds
long long calculateResponseTime(struct timeval start, struct timeval end) {
    return (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);
}

//Calculate average response time per function
void AverageResponseTime(const char *algorithmName) {
    long long total_response_time = 0;
    for (int i = 0; i < 4; i++) {
        total_response_time += calculateResponseTime(start_time[i], end_time[i]);
    }
    printf("%s - Average Response Time: %lld us\n\n", algorithmName, total_response_time / 4);
}

//Function to execute Round Robin Scheduling
void RR_Scheduling() {
    int processes = 4;
    for (int i = 0; i < 4; i++) {
        gettimeofday(&start_time[i], NULL);
    }
    while (processes > 0) {
        for (int i = 0; i < 4; i++) {
            if (remaining[i] > 0) {
                kill(pids[i], SIGCONT);
                usleep(TQ_RR);
                kill(pids[i], SIGSTOP);
                remaining[i] -= TQ_RR;
                if (waitpid(pids[i], NULL, WNOHANG) > 0) {
                    processes--;
                    gettimeofday(&end_time[i], NULL);
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
    //Compares each remaining workload size and allows the shortest remaining to execute first
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
        }
    }
    AverageResponseTime("SJF");
}

// Function to compute First Come First Serve Scheduling Algorithm
void FCFS_Scheduling() {
    for (int i = 0; i < 4; i++) {
        gettimeofday(&start_time[i], NULL);
    }
    //Loops through each workload and waits for each to complete prior to terminating
    for (int i = 0; i < 4; i++) { 
        kill(pids[i], SIGCONT);
        waitpid(pids[i], NULL, 0);
        gettimeofday(&end_time[i], NULL);
    }
    AverageResponseTime("FCFS");
}

int main() {
    //Prompts user to input Time Quantums for RR and MLFQ Scheduling
    printf("Enter Time Quantum for Round Robin Scheduling: \n");
    scanf("%d", &TQ_RR);
    /*printf("Enter Time Quantum for MLFQ Scheduling: \n");
    scanf("%d", &TQ_MLFQ);*/
    
    create_processes();
    for (int i = 0; i < 4; i++) remaining[i] = workloads[i];
    printf("Running Round Robin\n");
    RR_Scheduling();
    
    for (int i = 0; i < 4; i++) remaining[i] = workloads[i];
    printf("Running SJF\n");
    SJF_Scheduling();
    
    for (int i = 0; i < 4; i++) remaining[i] = workloads[i];
    printf("Running FCFS\n");
    FCFS_Scheduling();
    
    /*for (int i = 0; i < 4; i++) remaining[i] = workloads[i];
    printf("Running MLFQ\n");
    MLFQ_Scheduling();*/
    return 0;
}
