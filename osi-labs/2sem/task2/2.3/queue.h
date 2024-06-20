#pragma once
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#define MAX_STRING_LENGTH 100
#define USE_MUTEX

typedef struct Node {
    struct Node *next;
    //pthread_mutex_t sync;
    int counter_swap, counter_asc, counter_dsc, counter_eq;
    pthread_spinlock_t sync;
    //pthread_rwlock_t sync;
    char value[MAX_STRING_LENGTH];
} Node;

typedef struct _Storage {
    Node *first;
    //pthread_mutex_t sync;
    pthread_spinlock_t sync;
    //pthread_rwlock_t sync;
    int capacity;
} Storage;

typedef struct _ThreadData {
    Storage *storage;
    //int *counter;
    int type;
} ThreadData;


Storage *init_storage(int capacity);

void add_node(Storage *storage, const char *value);

void fill_storage(Storage *storage);

void print_storage(Storage *storage);
