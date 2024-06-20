#include <stdio.h>
#include <pthread.h>
#include <unistd.h>


void* thread_func(void* arg) {
    int counter = 0;
    while (1) {
        counter++;
        pthread_testcancel();
    }
    return NULL;
}

int main() {
    pthread_t thread;
    pthread_create(&thread, NULL, thread_func, NULL);
  
    sleep(3);
  
    pthread_cancel(thread);
    pthread_join(thread, NULL);
  
    printf("Thread has been canceled.\n");
  
    return 0;
}