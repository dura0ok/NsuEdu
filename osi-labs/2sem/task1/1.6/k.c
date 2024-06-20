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
#include <linux/futex.h>

#define STACK_SIZE (1024 * 1024)

typedef int (*start_routine_t)(void *);

typedef struct {
    int futex;
    int detached;
    start_routine_t start_routine;
    void *arg;
} thread_t;

int futex_wait(int *futex) {
    return syscall(SYS_futex, futex, FUTEX_WAIT, 0, NULL, NULL, 0);
}

int futex_wake(int *futex) {
    return syscall(SYS_futex, futex, FUTEX_WAKE, 1, NULL, NULL, 0);
}

int thread_start(void *arg) {
    thread_t *thread = (thread_t *)arg;
    int res = thread->start_routine(thread->arg);

    if (!thread->detached) {
        // Wake up the main thread if it's waiting for this thread to join
        futex_wake(&thread->futex);
    }

    return res;
}

int thread_create(thread_t *thread, start_routine_t start_routine, void *arg) {
    void *stack = mmap(NULL, STACK_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (stack == MAP_FAILED) {
        perror("mmap");
        return -1;
    }

    thread->futex = 0;
    thread->detached = 0;
    thread->start_routine = start_routine;
    thread->arg = arg;

    int flags = CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_THREAD | CLONE_SYSVSEM;
    int pid = clone(thread_start, (char *)stack + STACK_SIZE, flags, thread);
    if (pid == -1) {
        perror("clone");
        return -1;
    }

    return 0;
}

int thread_join(thread_t *thread, int *retval) {
    if (thread->detached) {
        fprintf(stderr, "Cannot join a detached thread\n");
        return -1;
    }

    while (!thread->futex) {
        futex_wait(&thread->futex);
    }

    if (retval != NULL) {
        *retval = thread->futex;
    }

    return 0;
}

int thread_detach(thread_t *thread) {
    thread->detached = 1;
    return 0;
}

void thread_exit(int retval) {
    futex_wake(&retval);
}

int thread_func(void *arg) {
    printf("Thread started\n");

    int *value = (int *)arg;
    printf("Value: %d\n", *value);

    printf("Thread finished\n");
    thread_exit(0);
}

int main() {
    thread_t thread;
    int value = 42;

    if (thread_create(&thread, thread_func, &value) == -1) {
        fprintf(stderr, "Failed to create thread\n");
        return 1;
    }

    thread_detach(&thread);
    int status;
    waitpid(-1, &status, __WCLONE);

    printf("Main thread exited\n");

    return 0;
}

