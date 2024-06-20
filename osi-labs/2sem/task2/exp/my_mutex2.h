#ifndef MUTEX_H
#define MUTEX_H

typedef struct {
    int futex;
} my_mutex_t;

void my_mutex_init(my_mutex_t* mutex);
void my_mutex_lock(my_mutex_t* mutex);
void my_mutex_unlock(my_mutex_t* mutex);

#endif  // MUTEX_H
