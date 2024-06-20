#include <stdio.h>
#include <stdlib.h>
#include "mythread.h"

volatile int print_lock = 0;

void *work(void *arg) {
    for(int i = 0; i < 100; i++){
        while (__sync_lock_test_and_set(&print_lock, 1));
        printf("hello world %d\n", i);
        print_lock = 0; 
    }
    return (void *) 10;
}

void *work2(void *arg) {
    for(int i = 0; i < 10; i++){
        while (__sync_lock_test_and_set(&print_lock, 1)); 
        printf("ADS %d\n", i);
        print_lock = 0; 
    }
    return (void *) 111;
}


int main(int argc, char *argv[]) {
    mythread_t t, t2;
    mythread_create(&t, work, NULL);
    mythread_create(&t2, work2, NULL);
    int *res = malloc(sizeof(int));
    mythread_detach(&t2);
    int err = mythread_join(&t, (void**)res);
    if(err < 0){
        fprintf(stderr, "error in join\n");
    }
    printf("RETURNED %d \n", *res);
    free(res);
    mythread_exit();
    return 0;
}