#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>

void* thread_func(void* arg) {
    printf("Thread id: %ld\n", pthread_self());
    pthread_detach(pthread_self());
    pthread_exit(NULL);
}

int main() {
    while (1) {
        pthread_t thread;
        int result = pthread_create(&thread, NULL, thread_func, NULL);
        if(result != 0) {
            printf("Thread creation failed: %s\n", strerror(errno));
            break;
        }
    }

    return 0;
}