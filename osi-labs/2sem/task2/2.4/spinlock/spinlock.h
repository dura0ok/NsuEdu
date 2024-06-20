#pragma once
#include <pthread.h>

typedef struct {
    int lock;
    int owner;
    int count;
} custom_spinlock_t;

void custom_spinlock_init(custom_spinlock_t* spinlock);
int custom_spinlock_lock(custom_spinlock_t* spinlock);
int custom_spinlock_unlock(custom_spinlock_t* spinlock);
