#include <stdio.h>
#include <pthread.h>

void* thread_func(void* arg) {
    printf("Thread id: %ld\n", pthread_self());
    pthread_exit(NULL);
}

int main() {
    while (1) {
        pthread_t thread;
        pthread_create(&thread, NULL, thread_func, NULL);
        pthread_detach(thread);
    }

    return 0;
}