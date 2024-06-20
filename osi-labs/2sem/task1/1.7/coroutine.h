#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <ucontext.h>
#include <stdbool.h>

typedef struct coro_t_ coro_t;
typedef int (*coro_function_t)(coro_t *coro);

struct coro_t_ {
    coro_function_t     function;         
    ucontext_t          suspend_context;    // prev
    ucontext_t          resume_context;     // curr
    int                 yield_value;       
    bool                is_coro_finished;  
};

coro_t *coro_new(coro_function_t function);
int coro_resume(coro_t *coro);    
void coro_yield(coro_t *coro, int value);
void coro_free(coro_t *coro);
