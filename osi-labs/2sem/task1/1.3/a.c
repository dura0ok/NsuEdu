#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

struct MyStruct {
    int num;
    char* str;
};

void* threadFunc(void* arg) {
    struct MyStruct* data = (struct MyStruct*)arg;
    
    printf("num: %d, str: %s\n", data->num, data->str);
    
    return NULL;
}

int main() {
    struct MyStruct data;
    data.num = 42;
    data.str = "Hello, world!";
    
    pthread_t thread;
    pthread_create(&thread, NULL, threadFunc, (void*)&data);
    
    pthread_join(thread, NULL);
    
    return 0;
}