// Custom Mutex implementation
typedef struct {
    volatile int lock;
    pthread_t owner;
    int count;
} custom_mutex_t;

void custom_mutex_init(custom_mutex_t* mutex) {
    mutex->lock = 0;
    mutex->owner = pthread_self();
    mutex->count = 0;
}

void custom_mutex_lock(custom_mutex_t* mutex) {
    pthread_t self = pthread_self();

    if (pthread_equal(mutex->owner, self)) {
        // Recursive lock
        mutex->count++;
        return;
    }

    while (__sync_bool_compare_and_swap(&mutex->lock, 0, 1)) {}

    mutex->owner = self;
    mutex->count = 1;
}

void custom_mutex_unlock(custom_mutex_t* mutex) {
    pthread_t self = pthread_self();

    if (pthread_equal(mutex->owner, self)) {
        mutex->count--;

        if (mutex->count == 0) {
            mutex->owner = pthread_self();
            __sync_bool_compare_and_swap(&mutex->lock, 1, 0);
        }

        return;
    }

    // Handle error: Attempting to unlock a mutex not owned by the calling thread
    fprintf(stderr, "Error: Attempting to unlock a mutex not owned by the calling thread.\n");
    exit(EXIT_FAILURE);
}
