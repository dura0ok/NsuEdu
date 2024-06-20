#define _GNU_SOURCE

#include <assert.h>
#include "queue.h"
#include <unistd.h>

void lock(int is_mutex, queue_t *q) {
    if (is_mutex) {
        my_mutex_lock(&(q->mutex));
    } else {
        spinlock_lock(&(q->spinlock));
    }
}

void unlock(int is_mutex, queue_t *q) {
    if (is_mutex) {
        my_mutex_unlock(&(q->mutex));
    } else {
        spinlock_unlock(&(q->spinlock));
    }
}


void *qmonitor(void *arg) {
    queue_t *q = (queue_t *)arg;

    printf("qmonitor: [%d %d %d]\n", getpid(), getppid(), gettid());

    while (1) {
        // Assuming queue_print_stats uses its own locking mechanism
        queue_print_stats(q);
        usleep(10000);
    }

    return NULL;
}

queue_t *queue_init(int max_count) {
    int err;

    queue_t *q = malloc(sizeof(queue_t));
    if (!q) {
        printf("Cannot allocate memory for a queue\n");
        abort();
    }

    q->first = NULL;
    q->last = NULL;
    q->max_count = max_count;
    q->count = 0;

    q->add_attempts = q->get_attempts = 0;
    q->add_count = q->get_count = 0;

    err = pthread_create(&q->qmonitor_tid, NULL, qmonitor, q);
    if (err) {
        printf("queue_init: pthread_create() failed: %s\n", strerror(err));
        abort();
    }

    my_mutex_init(&q->mutex);
    my_spinlock_init(&q->spinlock);
    return q;
}

void queue_destroy(queue_t *q) {
    qnode_t *current = q->first;
    qnode_t *next;

    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }


    free(q);
}

int queue_add(queue_t *q, int val) {
    my_mutex_lock(&q->mutex);
    q->add_attempts++;

    qnode_t *new = malloc(sizeof(qnode_t));
    if (!new) {
        printf("Cannot allocate memory for a new node\n");
        abort();
    }

    new->val = val;
    new->next = NULL;

    if (q->count == q->max_count) {
        my_mutex_unlock(&q->mutex);
        return 0;
    }

    //usleep(0);

    if (!q->first)
        q->first = q->last = new;
    else {
        q->last->next = new;
        q->last = q->last->next;
    }

    q->count++;
    q->add_count++;

    my_mutex_unlock(&q->mutex);

    return 1;
}

int queue_get(queue_t *q, int *val) {
    my_mutex_lock(&q->mutex);
    q->get_attempts++;

    qnode_t *tmp = q->first;
    if (tmp != NULL) {
        q->first = tmp->next;
        q->count--;
        q->get_count++;

        my_mutex_unlock(&q->mutex);

        *val = tmp->val;
        free(tmp);
        return 1;
    }

    my_mutex_unlock(&q->mutex);

    return 0;
}

void queue_print_stats(queue_t *q) {
    printf("queue stats: current size %d; attempts: (%ld %ld %ld); counts (%ld %ld %ld)\n",
           q->count,
           q->add_attempts, q->get_attempts, q->add_attempts - q->get_attempts,
           q->add_count, q->get_count, q->add_count - q->get_count);
}
