// vi: ts=4:sts=4:sw=4:et:tw=78

#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include <signal.h>
#include <stdalign.h>
#include <assert.h>

typedef struct {
    ucontext_t enter, leave;
    void (*start_routine)(void*);
    void *arg;
    alignas(long) char stk[SIGSTKSZ];
} co_t;

void co_trampoline(co_t *co) {
    co->start_routine(co->arg);
    setcontext(&co->leave);
}

co_t *co_create(void (*start_routine)(void*), void *arg) {
    co_t *co = malloc(sizeof *co);
    assert(co);
    getcontext(&co->enter);
    co->enter.uc_link = NULL;
    co->enter.uc_stack.ss_sp = co->stk;
    co->enter.uc_stack.ss_size = sizeof co->stk;
    co->start_routine = start_routine;
    makecontext(&co->enter, (void(*)(void))co_trampoline, 1, co);
    co->arg = arg;
    return co;
}
void co_call(co_t *co) {
    swapcontext(&co->leave, &co->enter);
}

// Test example
void print_message(void* message) {
    char* msg = (char*)message;
    printf("Thread message: %s\n", msg);
}

int main() {
    co_t *co;
    char message[] = "Hello, custom thread!";
    
    co = co_create(print_message, (void*)message);
    printf("Custom thread created successfully...\n");
    co_call(co);
    printf("Custom thread completed...\n");
    free(co);
    return 0;
}