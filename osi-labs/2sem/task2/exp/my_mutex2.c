#include "my_mutex2.h"
#include <unistd.h>
#include <sys/syscall.h>
#include <linux/futex.h>
#include <stdatomic.h>

void my_mutex_init(my_mutex_t* mutex) {
    mutex->futex = 0;
}

void my_mutex_lock(my_mutex_t* mutex) {
    int expected = 0;
    int val = __sync_val_compare_and_swap(&mutex->futex, expected, 1);

    while (val != expected) {
        syscall(SYS_futex, &mutex->futex, FUTEX_WAIT, 1, NULL, NULL, 0);
        val = __sync_val_compare_and_swap(&mutex->futex, expected, 1);
    }
}

void my_mutex_unlock(my_mutex_t* mutex) {
    __sync_val_compare_and_swap(&mutex->futex, 1, 0);
    syscall(SYS_futex, &mutex->futex, FUTEX_WAKE, 1, NULL, NULL, 0);
}
