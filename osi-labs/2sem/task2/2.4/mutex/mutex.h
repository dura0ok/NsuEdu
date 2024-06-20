#pragma once
#include <pthread.h>

typedef struct {
    int lock;
    pthread_t owner;
    int count;
} custom_mutex_t;

void custom_mutex_init(custom_mutex_t* mutex);
int custom_mutex_lock(custom_mutex_t* mutex);
int custom_mutex_unlock(custom_mutex_t* mutex);
