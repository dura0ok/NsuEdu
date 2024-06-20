#ifndef SPINLOCK_H
#define SPINLOCK_H

typedef struct {
    int lock;
} my_spinlock_t;

void spinlock_init(my_spinlock_t* lock);
void spinlock_lock(my_spinlock_t* lock);
void spinlock_unlock(my_spinlock_t* lock);

#endif // SPINLOCK_H
