#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

#define MAX_STRING_LENGTH 100

typedef struct _Node {
    char value[MAX_STRING_LENGTH];
    struct _Node* next;
    pthread_mutex_t sync;
} Node;

typedef struct _Storage {
    Node *first;
} Storage;

int ascendingCounter = 0;
int descendingCounter = 0;
int equalLengthCounter = 0;
int swapCounter = 0;

void initializeList(Storage *storage) {
    storage->first = NULL;
}

void appendToList(Storage *storage, const char *value) {
    Node *newNode = (Node*)malloc(sizeof(Node));
    if (newNode == NULL) {
        perror("Failed to allocate memory for a new node");
        exit(EXIT_FAILURE);
    }

    strncpy(newNode->value, value, MAX_STRING_LENGTH - 1);
    newNode->value[MAX_STRING_LENGTH - 1] = '\0';
    newNode->next = NULL;
    pthread_mutex_init(&newNode->sync, NULL);

    if (storage->first == NULL) {
        storage->first = newNode;
    } else {
        Node *current = storage->first;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = newNode;
    }
}

void printList(Storage *storage) {
    Node *current = storage->first;
    while (current != NULL) {
        printf("%s ", current->value);
        current = current->next;
    }
    printf("\n");
}

void swapNodes(Node *prev, Node *cur, Node *next) {
    if (prev == NULL || cur == NULL || next == NULL) {
        return;
    }

    pthread_mutex_lock(&prev->sync);
    pthread_mutex_lock(&cur->sync);
    pthread_mutex_lock(&next->sync);

    prev->next = next;
    cur->next = next->next;
    next->next = cur;

    pthread_mutex_unlock(&next->sync);
    pthread_mutex_unlock(&cur->sync);
    pthread_mutex_unlock(&prev->sync);
}

void* swapThreadFunction(void *arg) {
    Storage *storage = (Storage*)arg;

    while (1) {
        pthread_testcancel(); 

        Node *prev = NULL;
        Node *cur = storage->first;

        if (cur != NULL) {
            Node *next = cur->next;

            while (next != NULL && rand() % 2 != 0) {
                prev = cur;
                cur = next;
                next = next->next;
            }

            if (next != NULL) {
                swapNodes(prev, cur, next);
                swapCounter++;
            }
        }

        usleep(100000); 
        pthread_testcancel();  // Explicit cancellation point
    }

    pthread_exit(NULL);
}

void* ascendingThreadFunction(void *arg) {
    Storage *storage = (Storage*)arg;

    while (1) {
        pthread_testcancel();

        if (storage->first == NULL) {
            usleep(100000);
            continue;
        }

        Node *prev = storage->first;
        Node *current = prev->next;

        while (current != NULL) {
            pthread_mutex_lock(&prev->sync);
            pthread_mutex_lock(&current->sync);

            if (strlen(prev->value) < strlen(current->value)) {
                ascendingCounter++;
            }

            pthread_mutex_unlock(&current->sync);
            pthread_mutex_unlock(&prev->sync);

            prev = current;
            current = current->next;
        }

        usleep(100000);
        pthread_testcancel();  // Explicit cancellation point
    }

    pthread_exit(NULL);
}

void* descendingThreadFunction(void *arg) {
    Storage *storage = (Storage*)arg;

    while (1) {
        pthread_testcancel();

        if (storage->first == NULL) {
            usleep(100000);
            continue;
        }

        Node *prev = storage->first;
        Node *current = prev->next;

        while (current != NULL) {
            pthread_mutex_lock(&prev->sync);
            pthread_mutex_lock(&current->sync);

            if (strlen(prev->value) > strlen(current->value)) {
                descendingCounter++;
            }

            pthread_mutex_unlock(&current->sync);
            pthread_mutex_unlock(&prev->sync);

            prev = current;
            current = current->next;
        }

        usleep(100000);
        pthread_testcancel();  // Explicit cancellation point
    }

    pthread_exit(NULL);
}

void* equalLengthThreadFunction(void *arg) {
    Storage *storage = (Storage*)arg;

    while (1) {
        pthread_testcancel();

        if (storage->first == NULL) {
            usleep(100000);
            continue;
        }

        Node *prev = storage->first;
        Node *current = prev->next;

        while (current != NULL) {
            pthread_mutex_lock(&prev->sync);
            pthread_mutex_lock(&current->sync);

            if (strlen(prev->value) == strlen(current->value)) {
                equalLengthCounter++;
            }

            pthread_mutex_unlock(&current->sync);
            pthread_mutex_unlock(&prev->sync);

            prev = current;
            current = current->next;
        }

        usleep(100000);
        pthread_testcancel();  // Explicit cancellation point
    }

    pthread_exit(NULL);
}

int main() {
    srand(time(NULL));

    Storage storage;
    initializeList(&storage);

    pthread_t ascendingThread, descendingThread, equalLengthThread, swapThread1, swapThread2, swapThread3;

    if (pthread_create(&ascendingThread, NULL, ascendingThreadFunction, &storage) != 0 ||
        pthread_create(&descendingThread, NULL, descendingThreadFunction, &storage) != 0 ||
        pthread_create(&equalLengthThread, NULL, equalLengthThreadFunction, &storage) != 0 ||
        pthread_create(&swapThread1, NULL, swapThreadFunction, &storage) != 0 ||
        pthread_create(&swapThread2, NULL, swapThreadFunction, &storage) != 0 ||
        pthread_create(&swapThread3, NULL, swapThreadFunction, &storage) != 0) {
        perror("Failed to create threads");
        exit(EXIT_FAILURE);
    }

    appendToList(&storage, "apple");
    appendToList(&storage, "banana");
    appendToList(&storage, "orange");
    appendToList(&storage, "grape");
    appendToList(&storage, "kiwi");

    printList(&storage);

    sleep(5); // Let the threads work for 5 seconds

    // Gracefully terminate threads
    pthread_cancel(ascendingThread);
    pthread_cancel(descendingThread);
    pthread_cancel(equalLengthThread);
    pthread_cancel(swapThread1);
    pthread_cancel(swapThread2);
    pthread_cancel(swapThread3);

    // Allow some time for threads to finish
    sleep(1);

    // Join the threads after allowing them to finish
    if (pthread_join(ascendingThread, NULL) != 0 ||
        pthread_join(descendingThread, NULL) != 0 ||
        pthread_join(equalLengthThread, NULL) != 0 ||
        pthread_join(swapThread1, NULL) != 0 ||
        pthread_join(swapThread2, NULL) != 0 ||
        pthread_join(swapThread3, NULL) != 0) {
        perror("Failed to join threads");
        exit(EXIT_FAILURE);
    }

    return 0;
}
