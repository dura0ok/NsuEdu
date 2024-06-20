#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include <signal.h>

typedef struct {
    ucontext_t context;
    void (*start_routine)(void*);
    void *arg;
    int is_completed;
} custom_thread;

void custom_thread_start(void *args) {
    custom_thread* thread = (custom_thread*)args;
    thread->start_routine(thread->arg);
    thread->is_completed = 1;
    swapcontext(&thread->context, thread->context.uc_link);
}

int custom_thread_create(custom_thread *thread,  void (*start_routine)(void*), void *arg) {
    ucontext_t main_context;
    getcontext(&main_context);

    thread->context.uc_stack.ss_sp = malloc(SIGSTKSZ);
    thread->context.uc_stack.ss_size = SIGSTKSZ;
    thread->context.uc_link = &main_context;

    thread->start_routine = start_routine;
    thread->arg = arg;
    thread->is_completed = 0;

    makecontext(&thread->context, (void (*)(void))custom_thread_start, 1, thread);
    swapcontext(&main_context, &thread->context);
    return 0;
}

void print_message(void* message) {
    char* msg = (char*)message;
    printf("Thread message: %s\n", msg);
}

int main() {
    custom_thread thread;
    char message[] = "Hello, custom thread!";
    printf("starting\n");
    custom_thread_create(&thread, print_message, (void*)message);
    printf("Custom thread completed!...\n");
    free(thread.context.uc_stack.ss_sp);

    return 0;
}