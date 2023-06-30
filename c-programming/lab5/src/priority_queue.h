#pragma once

#include <stdbool.h>
#include <stdio.h>


typedef struct Node {
    int element;
    size_t priority;
    struct Node *left;
    struct Node *right;
} Node;

typedef struct PriorityQueue {
    int maxSize;
    int end;
    Node *array[];
} PriorityQueue;

Node *create_node(int element, size_t priority, Node *left, Node *right);

PriorityQueue *newPriorityQueue(int maxSize);

int size(PriorityQueue *queue);

bool isEmpty(PriorityQueue *queue);

bool isFull(PriorityQueue *queue);

bool enqueue(PriorityQueue *queue, Node *item);

Node *dequeue(PriorityQueue *queue);

void free_queue(PriorityQueue *queue);

Node *front(PriorityQueue *queue);


