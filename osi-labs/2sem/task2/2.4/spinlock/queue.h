#pragma once
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include "spinlock.h"

#define MAX_STRING_LENGTH 100
#define USE_spinlock

typedef struct Node {
    struct Node *next;
    custom_spinlock_t sync;
    int counter_swap, counter_asc, counter_dsc, counter_eq;
    char value[MAX_STRING_LENGTH];
} Node;

typedef struct _Storage {
    Node *first;
    custom_spinlock_t sync;
    int capacity;
} Storage;

typedef struct _ThreadData {
    Storage *storage;
    int type;
} ThreadData;


Storage *init_storage(int capacity);

void add_node(Storage *storage, const char *value);

void fill_storage(Storage *storage);

void print_storage(Storage *storage);
