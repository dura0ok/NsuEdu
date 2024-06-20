#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
 
void cleanup_handler(void* arg) {
    printf("cleaned");
    free(arg); 
}
 
void* print_hello(void* arg) {
    char* str = (char*)malloc(12); 
    pthread_cleanup_push(cleanup_handler, str);
 
    while (1) {
        strcpy(str, "hello world");
        printf("%s\n", str);
        usleep(1000000); 
    }
 
    pthread_cleanup_pop(1); 
    return NULL;
}
 
int main() {
    pthread_t tid;
    pthread_create(&tid, NULL, print_hello, NULL);
 
    
    sleep(5);
    pthread_cancel(tid);
    pthread_join(tid, NULL);
 
    return 0;
}