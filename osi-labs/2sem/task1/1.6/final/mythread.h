#pragma once

#include <setjmp.h>

typedef struct {
    jmp_buf env;
    void *stack;
    void *(*start_routine)(void *);
    void *arg;
    void *res;
    int completed;
    int joined;
    long tid;
    int active;
} mythread_t;

int mythread_create(mythread_t *thread, void *(*start_routine)(void *), void *arg);

void* routine_wrapper(void *arg);

void mythread_detach(mythread_t *t);

void mythread_exit();

int mythread_join(mythread_t *thread, void **ret_val);

int mythread_init();