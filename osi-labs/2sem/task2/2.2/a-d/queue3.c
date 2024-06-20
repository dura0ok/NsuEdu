#define _GNU_SOURCE
#include <pthread.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "queue.h"

void *qmonitor(void *arg) {
    queue_t *q = (queue_t *)arg;

    printf("qmonitor: [%d %d %d]\n", getpid(), getppid(), gettid());

    while (1) {
        queue_print_stats(q);
        sleep(1);
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

    if (pthread_mutex_init(&q->mutex, NULL) != 0) {
        perror("pthread_mutex_init");
        abort();
    }

    if (pthread_cond_init(&q->not_full, NULL) != 0) {
        perror("pthread_cond_init");
        abort();
    }

    if (pthread_cond_init(&q->not_empty, NULL) != 0) {
        perror("pthread_cond_init");
        abort();
    }

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

    pthread_mutex_destroy(&q->mutex);
    pthread_cond_destroy(&q->not_full);
    pthread_cond_destroy(&q->not_empty);
    free(q);
}

int queue_add(queue_t *q, int val) {
  


    pthread_mutex_lock(&q->mutex);
    
    q->add_attempts++;

    assert(q->count <= q->max_count);

    while (q->count == q->max_count) {
        pthread_cond_wait(&q->not_full, &q->mutex);
    }

    qnode_t *new = malloc(sizeof(qnode_t));
    if (!new) {
        printf("Cannot allocate memory for new node\n");
        abort();
    }

    new->val = val;
    new->next = NULL;

    if (!q->first)
        q->first = q->last = new;
    else {
        q->last->next = new;
        q->last = q->last->next;
    }
    q->count++;
    q->add_count++;

    pthread_cond_signal(&q->not_empty);
    pthread_mutex_unlock(&q->mutex);

    return 1;
}

int queue_get(queue_t *q, int *val) {
    q->get_attempts++;

    pthread_mutex_lock(&q->mutex);

    while (q->count == 0) {
        pthread_cond_wait(&q->not_empty, &q->mutex);
    }

    qnode_t *tmp = q->first;
    q->first = tmp->next;
    q->count--;
    q->get_count++;

    *val = tmp->val;
    free(tmp);

    pthread_cond_signal(&q->not_full);
    pthread_mutex_unlock(&q->mutex);

    return 1;
}

void queue_print_stats(queue_t *q) {
    printf("queue stats: current size %d; attempts: (%ld %ld %ld); counts (%ld %ld %ld)\n",
           q->count,
           q->add_attempts, q->get_attempts, q->add_attempts - q->get_attempts,
           q->add_count, q->get_count, q->add_count - q->get_count);
}
