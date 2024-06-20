#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include <signal.h>
#include <unistd.h>  
#include <sys/time.h>
#include <time.h> 
 
#define MAX_THREADS 5
 


ucontext_t thread_ctx[MAX_THREADS];  
int current_thread = 0;            

void thread_function(void* arg) {
    int tid = (int)arg;
    while (1) {
        printf("Thread %d is running\n", tid);
        sleep(1);
    }
}

// Function to switch to the next thread
void switch_thread() {
    int prev_thread = current_thread;
    current_thread = rand() % MAX_THREADS;
    swapcontext(&thread_ctx[prev_thread], &thread_ctx[current_thread]);
}

// Signal handler to switch threads
void signal_handler(int signum) {
    switch_thread();
}

int pthread_create(int tid, void* start_routine, void* arg) {
    ucontext_t* thread = &thread_ctx[tid];

    // Initialize the ucontext for the thread
    if (getcontext(thread) == -1) {
        perror("getcontext");
        return -1;
    }
    thread->uc_stack.ss_sp = malloc(8192);  // Allocate stack space
    thread->uc_stack.ss_size = 8192;
    thread->uc_link = NULL;
    makecontext(thread, (void (*)())start_routine, 1, (int)arg);

    return 0;  // Success
}

int main() {
    srand(time(NULL));  // Initialize random seed

    // Creating custom threads
    for (int i = 0; i < MAX_THREADS; i++) {
        if (pthread_create(i, thread_function, (void*)i) != 0) {
            fprintf(stderr, "Error creating thread %d\n", i);
            exit(1);
        }
    }

    // Install signal handler to handle switching threads
    signal(SIGALRM, signal_handler);

    // Set alarm to occur every second
    struct itimerval timer;
    timer.it_interval.tv_sec = 1;
    timer.it_interval.tv_usec = 0;
    timer.it_value = timer.it_interval;
    setitimer(ITIMER_REAL, &timer, NULL);

    // Start the first thread
    setcontext(&thread_ctx[current_thread]);

    return 0;
}