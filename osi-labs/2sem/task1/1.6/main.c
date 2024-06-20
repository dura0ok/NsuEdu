#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/syscall.h>

#define STACK_SIZE BUFSIZ

typedef struct {
    int completed;
    void *(*start_routine)(void *);
    void *arg;
    void *stack;
} mythread_t;

void *mythread_start(void *arg) {
    mythread_t *thread = (mythread_t *)arg;
    thread->start_routine(thread->arg);
    thread->completed = 1;
    return NULL;
}

int mythread_create(mythread_t *thread, void *(*start_routine)(void *), void *arg) {
    thread->completed = 0;
    thread->start_routine = start_routine;
    thread->arg = arg;
    thread->stack = mmap(NULL, STACK_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (thread->stack == MAP_FAILED) {
        perror("mmap");
        return -1;
    }
    if (clone((int (*)(void *)) mythread_start, (char *)thread->stack + STACK_SIZE, CLONE_THREAD | CLONE_VM | CLONE_SIGHAND | CLONE_FS | CLONE_FILES, thread) == -1) {
        perror("clone");
        return -1;
    }
    return 0;
}

int mythread_join(mythread_t *thread) {
    while (!thread->completed) {
        sched_yield();
    }
    if (munmap(thread->stack, STACK_SIZE) == -1) {
        perror("munmap");
        return -1;
    }
    return 0;
}

int mythread_detach(mythread_t *thread) {
    if (munmap(thread->stack, STACK_SIZE) == -1) {
        perror("munmap");
        return -1;
    }
    return 0;
}

void *my_function(void *arg) {
    int *value = (int *)arg;
    printf("Thread value: %d\n", *value);
    return NULL;
}

int main() {
    mythread_t thread;
    int value = 10;

    if (mythread_create(&thread, my_function, &value) != 0) {
        fprintf(stderr, "Failed to create thread\n");
        return 1;
    }

    if (mythread_join(&thread) != 0) {
        fprintf(stderr, "Failed to join thread\n");
        return 1;
    }

    return 0;
}