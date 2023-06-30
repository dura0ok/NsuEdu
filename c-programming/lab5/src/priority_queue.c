#include "priority_queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

Node *create_node(int element, size_t priority, Node *left, Node *right) {
    Node *item = malloc(sizeof(Node));
    item->element = element;
    item->priority = priority;
    item->left = left;
    item->right = right;
    return item;
}

PriorityQueue *newPriorityQueue(int maxSize) {
    if (maxSize < 1) {
        printf("Error: maximum size must be positive.\n");
        return NULL;
    }

    PriorityQueue *ptr =
            malloc(sizeof(PriorityQueue) + maxSize * sizeof(Node));

    ptr->maxSize = maxSize;
    ptr->end = -1;

    return ptr;
}


int size(PriorityQueue *queue) { return queue->end + 1; }


bool isEmpty(PriorityQueue *queue) { return size(queue) == 0; }


bool isFull(PriorityQueue *queue) { return size(queue) == queue->maxSize; }


bool enqueue(PriorityQueue *queue, Node *item) {
    if (isFull(queue)) {
        printf("Enqueue error: priority queue is already full.\n");
        return 1;
    }

    // Move elements toward the end until the correct insert index is found
    int insertIndex = size(queue);
    while (insertIndex > 0 && queue->array[insertIndex - 1]->priority > item->priority) {
        queue->array[insertIndex] = queue->array[insertIndex - 1];
        insertIndex--;
    }

    queue->array[insertIndex] = item;
    queue->end++;

    return 0;
}


Node *front(PriorityQueue *queue) {
    if (isEmpty(queue)) {
        return NULL;
    }

    return queue->array[0];
}

Node *dequeue(PriorityQueue *queue) {
    if (isEmpty(queue)) {
        printf("Dequeue error: priority queue is already empty.\n");
        assert(0);
    }

    Node *dequeued = queue->array[0];

    for (int i = 0; i < queue->end; i++) {
        queue->array[i] = queue->array[i + 1];
    }

    queue->end--;

    return dequeued;
}


void delete_item(Node *item) {
    if (item == NULL) return;

    delete_item(item->left);
    delete_item(item->right);
    free(item);
}

void free_queue(PriorityQueue *queue) {
    for (size_t i = 0; i < (size_t) size(queue); ++i) {
        delete_item(queue->array[i]);
    }
    free(queue);
}


