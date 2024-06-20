#include <unistd.h>
#include <linux/futex.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <stdio.h>
#include "spinlock.h"


void custom_spinlock_init(custom_spinlock_t* spinlock) {
    spinlock->lock = 0;
}

int custom_spinlock_lock(custom_spinlock_t* spinlock) {
    while (!__sync_bool_compare_and_swap(&spinlock->lock, 0, 1)) {}
}

int custom_spinlock_unlock(custom_spinlock_t* spinlock) {
   __sync_bool_compare_and_swap(&spinlock->lock, 1, 0);
}
