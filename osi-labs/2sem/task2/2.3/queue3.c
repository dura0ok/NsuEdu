#include "queue.h"

Storage *init_storage(int capacity) {
    Storage *storage = (Storage*)malloc(sizeof(Storage));
    if (!storage) {
        printf("Failed to allocate memory for a queue\n");
        exit(1);
    }
    pthread_rwlock_init(&(storage->sync), NULL);
    storage->capacity = capacity;
    storage->first = NULL;
    return storage;
}

Node *create_node(const char *value) {
    Node *new_node = (Node *) calloc(1, sizeof(*new_node));
    if (!new_node) {
        perror("Failed to allocate memory for a new node");
        exit(EXIT_FAILURE);
    }

    strcpy(new_node->value, value);
    pthread_rwlock_init(&(new_node->sync), NULL);

    return new_node;
}

void add_node(Storage *storage, const char *value) {
    Node *new_node = create_node(value);

    pthread_rwlock_wrlock(&(storage->sync)); // Write lock

    if (storage->first != NULL) {
        Node *node = storage->first;
        while (node->next != NULL) {
            node = node->next;
        }
        node->next = new_node;
    } else {
        storage->first = new_node;
    }

    pthread_rwlock_unlock(&(storage->sync)); // Unlock
}

void fill_storage(Storage *storage) {
    char buff[24];
    for (int i = 1; i <= storage->capacity; ++i) {
        int digits = snprintf(NULL, 0, "%d", i % (storage->capacity + 1));
        sprintf(buff, "%d", i % (storage->capacity + 1));
        add_node(storage, buff);
    }
}

void print_storage(Storage *storage) {
    pthread_rwlock_rdlock(&(storage->sync)); // Read lock

    Node *current = storage->first;
    while (current != NULL) {
        printf("%s ", current->value);
        current = current->next;
    }
    printf("\n");

    pthread_rwlock_unlock(&(storage->sync)); // Unlock
}
