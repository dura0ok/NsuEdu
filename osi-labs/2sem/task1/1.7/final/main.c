#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <stdint.h>
#include <linux/futex.h>
#include <sys/syscall.h>
#include <errno.h>

#define MAX_THREADS 2

ucontext_t thread_ctx[MAX_THREADS];
int current_thread = 0;
int thread_completed[MAX_THREADS] = {0};
int futex_lock = 0;
int tv_usec;

int futex(int *uaddr, int futex_op, int val, const struct timespec *timeout, int *uaddr2, int val3) {
    return syscall(SYS_futex, uaddr, futex_op, val, timeout, uaddr2, val3);
}

void mark_thread_completed(int tid) {
    thread_completed[tid] = 1;
    futex(&futex_lock, FUTEX_WAKE_PRIVATE, 1, NULL, NULL, 0);
}

void thread_function(void* arg) {
    uintptr_t tid = (uintptr_t)arg;
    for(int i = 0; i < 10; i++) {
        printf("hi - tid %ld\n", tid);
    }

    mark_thread_completed(tid);
}

void signal_handler(int signum) {
    int prev_thread = current_thread;
    current_thread = (current_thread + 1) % MAX_THREADS;

    
    swapcontext(&thread_ctx[prev_thread], &thread_ctx[current_thread]);
    
}

int pthread_create(int tid, void* start_routine, void* arg) {
    ucontext_t* thread = &thread_ctx[tid];

    if (getcontext(thread) == -1) {
        perror("getcontext");
        return -1;
    }
    thread->uc_stack.ss_sp = malloc(8192);
    thread->uc_stack.ss_size = 8192;
    thread->uc_link = NULL;
    makecontext(thread, (void (*)())start_routine, 1, (uintptr_t)arg);

    return 0;
}

void pthread_join(int tid) {
    if (!thread_completed[tid]) {
        futex(&futex_lock, FUTEX_WAIT_PRIVATE, 0, NULL, NULL, 0);
    }
    
}

int main() {
    srand(time(NULL));
    
    // Retrieve the tv_usec value from an environment variable (e.g., TV_USEC)
    char* tv_usec_str = getenv("TV_USEC");
    if (tv_usec_str) {
        tv_usec = atoi(tv_usec_str);
    } else {
        fprintf(stderr, "TV_USEC environment variable not set.\n");
        exit(1);
    }

    int thread_ids[MAX_THREADS];

    for (int i = 0; i < MAX_THREADS; i++) {
        thread_ids[i] = i;
        if (pthread_create(i, thread_function, (void *)(uintptr_t)i) != 0) {
            fprintf(stderr, "Error creating thread %d\n", i);
            exit(1);
        }
    }

    struct sigaction sa;
    sa.sa_handler = signal_handler;
    sa.sa_flags = SA_NODEFER;
    sigaction(SIGALRM, &sa, NULL);

    struct itimerval timer;
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = tv_usec;
    timer.it_value = timer.it_interval;
    setitimer(ITIMER_REAL, &timer, NULL);

    thread_function((void *)(uintptr_t)current_thread);

    for (int i = 0; i < MAX_THREADS; i++) {
        pthread_join(thread_ids[i]);
    }

    return 0;
}
