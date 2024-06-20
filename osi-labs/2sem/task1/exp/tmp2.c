#define _GNU_SOURCE
#include <stdio.h>
#include <sched.h>
#include <stdlib.h>
#include <setjmp.h>
#include <signal.h>
#include <unistd.h>

#define STACK_SIZE 4096
#define MAX_THREADS 128

typedef int mythread_t;

typedef struct mythread_struct {
    void *(*start_routine)(void*);
    void *arg;
    void *stack;
    int tid;
    jmp_buf context;
    int is_detached;
} mythread_struct;

static mythread_struct *threads[MAX_THREADS];
static int current_thread_index = 0;

// Simple lock mechanism
static volatile int lock = 0;

static inline void simple_lock() {
    while(__sync_lock_test_and_set(&lock, 1));
}

static inline void simple_unlock() {
    __sync_synchronize();
    lock = 0;
}

// Thread wrapper
static int thread_start(void *arg) {
    mythread_struct *thread = (mythread_struct*)arg;
    thread->start_routine(thread->arg);
    thread->tid = -1; // Mark as done
    if (thread->is_detached) {
        free(thread->stack);
        free(thread);
    }
    return 0;
}

int mythread_create(mythread_t *thread, void *(*start_routine) (void *), void *arg) {
    simple_lock();

    mythread_struct *new_thread = malloc(sizeof(mythread_struct));
    new_thread->start_routine = start_routine;
    new_thread->arg = arg;
    new_thread->stack = malloc(STACK_SIZE);
    new_thread->is_detached = 0;

    int tid = clone(thread_start, new_thread->stack + STACK_SIZE,
                    CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND, new_thread);
    if (tid == -1) {
        free(new_thread->stack);
        free(new_thread);
        simple_unlock();
        return -1;
    }

    new_thread->tid = tid;
    *thread = current_thread_index;
    threads[current_thread_index++] = new_thread;

    simple_unlock();
    return 0;
}

int mythread_detach(mythread_t thread) {
    simple_lock();

    if (thread >= MAX_THREADS || !threads[thread] || threads[thread]->tid == -1) {
        simple_unlock();
        return -1;
    }

    threads[thread]->is_detached = 1;
    simple_unlock();
    return 0;
}

void mythread_exit(void *retval) {
    _exit(0);
}

void some_function(){
    while(1){
        printf("work thread\n");
    }
}
int main() {
    mythread_t thread;
    mythread_create(&thread, (void *(*)(void *)) some_function, NULL);
    mythread_detach(thread);
    mythread_exit(NULL);

    return 0;
}