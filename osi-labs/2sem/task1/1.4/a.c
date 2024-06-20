#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
 
void* print_strings(void* arg) {
    while (1) {
        printf("Printing...\n");
        usleep(1000000); // Sleep for 1 second
    }
    return NULL;
}
 
int main() {
    pthread_t tid;
    pthread_create(&tid, NULL, print_strings, NULL);

    sleep(5);
    pthread_cancel(tid);
    pthread_join(tid, NULL); 
 
    return 0;
}