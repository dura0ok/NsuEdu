#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

void* thread_function(void* arg) {
    for (int i = 0; i < 10; i++) {
        printf("Thread is running...\n");
        sleep(1);
    }

    pthread_testcancel();

    printf("Thread completed.\n");
    pthread_exit(NULL);
}

int main() {
    pthread_t thread;
    int result = pthread_create(&thread, NULL, thread_function, NULL);
    if (result != 0) {
        perror("Thread creation failed");
        exit(EXIT_FAILURE);
    }

    sleep(3);

    result = pthread_cancel(thread);
    if (result != 0) {
        perror("Thread cancellation failed");
        exit(EXIT_FAILURE);
    }

    void* thread_result;
    result = pthread_join(thread, &thread_result);
    if (result != 0) {
        perror("Thread join failed");
        exit(EXIT_FAILURE);
    }

    if (thread_result == PTHREAD_CANCELED) {
        printf("Thread was canceled with return code 0.\n");
    } else {
        printf("Thread completed with return code '%ld'.\n", (long)thread_result);
    }

    return 0;
}