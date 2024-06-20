#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#define MAX_STRING_LENGTH 100

typedef struct _Node {
    char value[MAX_STRING_LENGTH];
    struct _Node* next;
    pthread_mutex_t sync;
} Node;

typedef struct _Storage {
    Node* first;
    pthread_rwlock_t rwlock;  // для блокировок чтения-записи
} Storage;

typedef struct _ThreadData {
    Storage* storage;
    int* iterationCounter;
} ThreadData;

void initializeStorage(Storage* storage) {
    storage->first = NULL;
    pthread_rwlock_init(&storage->rwlock, NULL);
}

void insertNode(Storage* storage, const char* value) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (newNode == NULL) {
        perror("Memory allocation error");
        exit(EXIT_FAILURE);
    }

    strncpy(newNode->value, value, MAX_STRING_LENGTH - 1);
    newNode->value[MAX_STRING_LENGTH - 1] = '\0';  // Ensure null-terminated string

    pthread_mutex_init(&newNode->sync, NULL);

    pthread_rwlock_wrlock(&storage->rwlock);
    newNode->next = storage->first;
    storage->first = newNode;
    pthread_rwlock_unlock(&storage->rwlock);
}

void printList(Storage* storage) {
    pthread_rwlock_rdlock(&storage->rwlock);
    Node* current = storage->first;
    while (current != NULL) {
        printf("%s\n", current->value);
        current = current->next;
    }
    pthread_rwlock_unlock(&storage->rwlock);
}

void* ascendingThread(void* arg) {
    ThreadData* threadData = (ThreadData*)arg;
    Storage* storage = threadData->storage;
    int* iterationCounter = threadData->iterationCounter;

    while (1) {
        (*iterationCounter)++;
    }

    return NULL;
}

void* descendingThread(void* arg) {
    ThreadData* threadData = (ThreadData*)arg;
    Storage* storage = threadData->storage;
    int* iterationCounter = threadData->iterationCounter;

    while (1) {
        (*iterationCounter)++;
    }

    return NULL;
}

void* equalLengthThread(void* arg) {
    ThreadData* threadData = (ThreadData*)arg;
    Storage* storage = threadData->storage;
    int* iterationCounter = threadData->iterationCounter;

    while (1) {
        (*iterationCounter)++;
    }

    return NULL;
}

void* writerThread(void* arg) {
    Storage* storage = (Storage*)arg;

    while (1) {
     
    }

    return NULL;
}

int main() {
    Storage storage;
    initializeStorage(&storage);

    int iterationCounterAsc = 0;
    int iterationCounterDesc = 0;
    int iterationCounterEqual = 0;

    ThreadData ascendingData = {&storage, &iterationCounterAsc};
    ThreadData descendingData = {&storage, &iterationCounterDesc};
    ThreadData equalLengthData = {&storage, &iterationCounterEqual};

    pthread_t ascendingThreadId, descendingThreadId, equalLengthThreadId;
    pthread_t writerThreadId1, writerThreadId2, writerThreadId3;

    pthread_create(&ascendingThreadId, NULL, ascendingThread, &ascendingData);
    pthread_create(&descendingThreadId, NULL, descendingThread, &descendingData);
    pthread_create(&equalLengthThreadId, NULL, equalLengthThread, &equalLengthData);

    pthread_create(&writerThreadId1, NULL, writerThread, &storage);
    pthread_create(&writerThreadId2, NULL, writerThread, &storage);
    pthread_create(&writerThreadId3, NULL, writerThread, &storage);

    // Ждем завершения потоков
    pthread_join(ascendingThreadId, NULL);
    pthread_join(descendingThreadId, NULL);
    pthread_join(equalLengthThreadId, NULL);
    pthread_join(writerThreadId1, NULL);
    pthread_join(writerThreadId2, NULL);
    pthread_join(writerThreadId3, NULL);

    // Очищаем ресурсы
    pthread_rwlock_destroy(&storage.rwlock);

    return 0;
}
