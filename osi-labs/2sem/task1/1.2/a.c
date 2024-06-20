#include <pthread.h>
#include <stdio.h>

void* threadFunction(void* arg) {
    printf("child thread\n");
    pthread_exit(NULL);
}

int main() {
    pthread_t thread;

    pthread_create(&thread, NULL, threadFunction, NULL);

    pthread_join(thread, NULL);

    printf("main thread\n");

    return 0;
}
