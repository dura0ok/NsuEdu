#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

struct MyStruct {
    int num;
    char* str;
};

void* threadFunc(void* arg) {
    struct MyStruct* data = (struct MyStruct*)arg;
    
    printf("num: %d, str: %s\n", data->num, data->str);
    
    free(data);
    
    return NULL;
}

int main() {
    struct MyStruct* data = malloc(sizeof(struct MyStruct)); 
    data->num = 42;
    data->str = "Hello, world!";
    
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED); 
    
    pthread_t thread;
    pthread_create(&thread, &attr, threadFunc, (void*)data); 
    
    pthread_attr_destroy(&attr);
    
    
    sleep(10);
    return 0;
}

//  Если поток создается как detached поток, то структура должна располагаться в куче  (динамической памяти).
