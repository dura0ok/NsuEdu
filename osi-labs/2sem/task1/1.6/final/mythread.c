#define _GNU_SOURCE
#include "mythread.h"
#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <setjmp.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/syscall.h>
#include <linux/futex.h>
#include <stdbool.h>
#include <limits.h>
#include <stdint.h>
#include <signal.h>


#define STACK_SIZE (1 << 21)

volatile int NUM_ALIVE_THREADS = 1;
__thread void *current_thread_tls = NULL;

// Function to get the PID of the current thread
pid_t gettid() {
    return syscall(SYS_gettid);
}

void signal_handler(int signum) {
    write(1, "!!!!!\n", 6);
    if (current_thread_tls) {
        mythread_t *thread = (mythread_t *)current_thread_tls;
        if (thread->stack) {
            munmap(thread->stack, STACK_SIZE);
        }
    }
    _exit(0);
}


void* routine_wrapper(void *arg) {
    mythread_t* t = (mythread_t*)arg;
    current_thread_tls = t;
    t->res = t->start_routine(t->arg);
    t->completed = true;

    syscall(SYS_futex, &t->completed, FUTEX_WAKE, 1, NULL, NULL, 0);
    return NULL;
}

static bool initialized = false;  // Keep track of whether mythread_init has been called

int mythread_create(mythread_t *thread, void *(*start_routine)(void *), void *arg) {
    if (!initialized) {
        if (mythread_init() != 0) {
            fprintf(stderr, "Failed to initialize threading library.\n");
            return -1;
        }
        initialized = true;
    }

    thread->arg = arg;
    thread->stack = mmap(NULL, STACK_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_STACK, -1, 0);

    if (thread->stack == MAP_FAILED) {
        perror("mmap");
        return -1;
    }

    // Allocate a guard page
    mprotect(thread->stack, sysconf(_SC_PAGESIZE), PROT_NONE);

    thread->start_routine = start_routine;
    thread->joined = true;
    thread->completed = false;

    char *sp = (char *)thread->stack + STACK_SIZE;
    sp = (char *)((uintptr_t)sp & ~0xF); // 16-byte alignment

    if (clone((int (*)(void *))routine_wrapper, sp, CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_THREAD, thread) == -1) {
        perror("clone");
        munmap(thread->stack, STACK_SIZE);
        return -1;
    }

    __sync_fetch_and_add(&NUM_ALIVE_THREADS, 1);

    struct sched_param param;
    param.sched_priority = 0;
    if (sched_setparam(0, &param) == -1) {
        perror("sched_setparam");
        return -1;
    }

    return 0;
}

void mythread_detach(mythread_t *t) {
    t->joined = false;
}

int mythread_join(mythread_t *thread, void **ret_val) {
    if (!thread->joined) {
        return -1;
    }

    while (!thread->completed) {
        syscall(SYS_futex, &thread->completed, FUTEX_WAIT, 0, NULL, NULL, 0);
    }

    if (ret_val) {
        *ret_val = thread->res;
    }

    return 0;
}

void mythread_exit() {
    if (__sync_fetch_and_sub(&NUM_ALIVE_THREADS, 1) == 1) {
        tgkill(getpid(), gettid(), SIGUSR1);
    }
    syscall(SYS_futex, &NUM_ALIVE_THREADS, FUTEX_WAIT, 0, NULL, NULL, 0);
}

int mythread_init() {
    struct sigaction sa;
    sa.sa_handler = signal_handler;
    sa.sa_flags = SA_ONSTACK;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        perror("sigaction");
        return -1;
    }
    return 0;
}
