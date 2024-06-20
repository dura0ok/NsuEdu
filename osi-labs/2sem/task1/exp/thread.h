#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <setjmp.h>
#include <sys/mman.h>


typedef struct {
    int completed;
    void *(*start_routine)(void *);
    void *arg;
    void *stack;
    jmp_buf buf;
    pid_t tid;
    void *res;
} mythread_t;

static int NUM_THREADS = 10;

void thread_routine(mythread_t *thread, void *(*start_routine)(void *)) {
    thread->res = start_routine(thread->arg);
    thread->completed = 1;
}

int mythread_create(mythread_t *th, void *(*start_routine)(void *), void *arg) {
    if (NUM_THREADS >= 10) {
        fprintf(stderr, "Maximum number of threads reached.\n");
        return -1;
    }

    Thread *new_thread = &threads[num_threads];
    new_thread->start_routine = start_routine;
    new_thread->arg = arg;
    new_thread->finished = 0;
    new_thread->tid = num_threads;

    char *child_stack = mmap(NULL, STACK_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_GROWSDOWN, -1, 0);
    if (child_stack == MAP_FAILED) {
        fprintf(stderr, "Failed to allocate stack memory.\n");
        return -1;
    }

    int flags = SIGCHLD | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_VM;

    int rs = clone((int (*)(void *)) thread_routine,
                   (void *)(child_stack + STACK_SIZE),
                   flags,
                   (void *) new_thread->start_routine,
                   new_thread->arg);

    if (rs == -1) {
        fprintf(stderr, "Failed to create a new thread.\n");
        return -1;
    }

    *thread = *new_thread;
    num_threads++;
    return 0;
}