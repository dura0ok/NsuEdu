
#include "spinlock.h"

void spinlock_init(my_spinlock_t* lock) {
    lock->lock = 0;
}

void spinlock_lock(my_spinlock_t* lock) {
    while (!__sync_bool_compare_and_swap(&lock->lock, 0, 1)) {}
}

void spinlock_unlock(my_spinlock_t* lock) {
   __sync_bool_compare_and_swap(&lock->lock, 1, 0)
}
