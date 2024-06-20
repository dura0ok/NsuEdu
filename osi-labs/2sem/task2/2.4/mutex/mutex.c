#include <unistd.h>
#include <linux/futex.h>
#include <sys/syscall.h>
#include <stdio.h>
#include "mutex.h"

static inline int futex(int *uaddr, int futex_op, int val, const struct timespec *timeout, int *uaddr2, int val3) {
    return syscall(SYS_futex, uaddr, futex_op, val, timeout, uaddr2, val3);
}

void custom_mutex_init(custom_mutex_t* mutex) {
    mutex->lock = 0;
    mutex->owner = -1;
    mutex->count = 0;
}

int acquire_lock(pthread_t self, custom_mutex_t* mutex) {
    int expected = 0;
    if (__sync_bool_compare_and_swap(&mutex->lock, expected, 1)) {
        mutex->owner = self;
        mutex->count = 1;
        return 1;
    }
    return 0;
}

int custom_mutex_lock(custom_mutex_t* mutex) {
    pthread_t self = pthread_self();

    if (pthread_equal(mutex->owner, self)) {
        mutex->count++;
        return 0;
    }

    for (int i = 0; i < 100; i++) {
        if (acquire_lock(self, mutex)) {
            return 0;
        }
    }

    while (!acquire_lock(self, mutex)) {
        futex(&mutex->lock, FUTEX_WAIT, 1, NULL, NULL, 0);
    }

    return 0;
}



int custom_mutex_unlock(custom_mutex_t* mutex) {
    pthread_t self = pthread_self();

    if (pthread_equal(mutex->owner, self) && mutex->count > 0) {
        mutex->count--;

        if (mutex->count == 0) {
            mutex->owner = -1;
            __sync_lock_release(&mutex->lock);
            futex(&mutex->lock, FUTEX_WAKE, 1, NULL, NULL, 0);
        }

        return 0;
    }

    // Handle error: Attempting to unlock a mutex not owned by the calling thread
    fprintf(stderr, "Error: Attempting to unlock a mutex not owned by the calling thread.\n");
    return 1;
}
