#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include "queue.h"


#define STORAGE_CAPACITY 100
#define THREAD_COUNT 6
#define ASC 0
#define DESC 1
#define EQ 2
#define SWAP 3

void perform_swap(Node *curr1, Node *curr2, Node *curr3, int *swap_counter) {
    curr2->next = curr3->next;
    curr3->next = curr2;
    curr1->next = curr3;
    (*swap_counter)++;
}

void *compare_length_thread(void *data) {
    ThreadData *thread_data = (ThreadData *) data;
    Storage *storage = thread_data->storage;
    int *counter = thread_data->counter;
    int type = thread_data->type;

    while (1) {
        Node *curr = storage->first;
        if (curr == NULL || curr->next == NULL) {
            printf("Too few elements in the queue");
            break;
        }

        Node *curr2, *tmp;
        while (1) {
            if (curr != NULL && pthread_spin_lock(&curr->spinlock) == 0) {
                if (curr->next != NULL && pthread_spin_lock(&curr->next->spinlock) == 0) {
                    volatile int pair_count = 0;
                    curr2 = curr->next;

                    switch (type) {
                        case ASC:
                            pair_count = (strlen(curr->value) < strlen(curr2->value));
                            break;
                        case DESC:
                            pair_count = (strlen(curr->value) > strlen(curr2->value));
                            break;
                        case EQ:
                            pair_count = (strlen(curr->value) == strlen(curr2->value));
                            break;
                    }

                    tmp = curr;
                    curr = curr->next;
                    pthread_spin_unlock(&tmp->spinlock);
                    pthread_spin_unlock(&curr->spinlock);

                    if (pair_count) {
                        (*counter)++;
                    }
                } else {
                    tmp = curr;
                    curr = curr->next;
                    pthread_spin_unlock(&tmp->spinlock);
                }
            } else if (curr == NULL) {
                break;
            } else {
                curr = curr->next;
            }
        }
    }

    return NULL;
}

_Noreturn void *swap_thread(void *data) {
    ThreadData *thread_data = (ThreadData *) data;
    Storage *storage = thread_data->storage;
    int *swap_counter = thread_data->counter;

    while (1) {
        Node *curr1 = storage->first;
        Node *curr2, *curr3, *tmp;

        while (1) {
            if (curr1 != NULL && pthread_spin_lock(&curr1->spinlock) == 0) {
                if (curr1->next != NULL && pthread_spin_lock(&curr1->next->spinlock) == 0) {
                    if (curr1->next->next != NULL && pthread_spin_lock(&curr1->next->next->spinlock) == 0) {
                        curr2 = curr1->next;
                        curr3 = curr1->next->next;
                        if (rand() % 2 == 0) {
                            perform_swap(curr1, curr2, curr3, swap_counter);
                        }

                        tmp = curr1;
                        curr1 = tmp->next;
                        curr2 = curr1->next;
                        pthread_spin_unlock(&tmp->spinlock);
                        pthread_spin_unlock(&curr1->spinlock);
                        pthread_spin_unlock(&curr2->spinlock);
                    } else {
                        tmp = curr1;
                        curr1 = curr1->next;
                        pthread_spin_unlock(&tmp->spinlock);
                        pthread_spin_unlock(&curr1->spinlock);
                    }
                } else {
                    tmp = curr1;
                    curr1 = curr1->next;
                    pthread_spin_unlock(&tmp->spinlock);
                }
            } else if (curr1 == NULL) {
                break;
            } else {
                curr1 = curr1->next;
            }
        }
    }

}

_Noreturn void *count_monitor(void *arg) {
    int *counters = (int *) arg;
    while (1) {
        printf("ASC: %d, DESC: %d, EQ: %d, SWAP: %d\n",
               counters[ASC], counters[DESC], counters[EQ], counters[SWAP]);
        sleep(1);
    }
}

int main() {
    Storage *storage = init_storage(STORAGE_CAPACITY);
    fill_storage(storage);
    print_storage(storage);

    pthread_t compare_asc_tid, compare_desc_tid, compare_eq_tid, swap_tid1, swap_tid2, swap_tid3, monitor;

    int *counters = calloc(THREAD_COUNT, sizeof(int));

    ThreadData compare_asc_data = {storage, &counters[ASC], ASC};
    ThreadData compare_desc_data = {storage, &counters[DESC], DESC};
    ThreadData compare_eq_data = {storage, &counters[EQ], EQ};
    ThreadData swap_data1 = {storage, &counters[SWAP]};
    ThreadData swap_data2 = {storage, &counters[SWAP]};
    ThreadData swap_data3 = {storage, &counters[SWAP]};

    pthread_create(&compare_asc_tid, NULL, compare_length_thread, &compare_asc_data);
    pthread_create(&compare_desc_tid, NULL, compare_length_thread, &compare_desc_data);
    pthread_create(&compare_eq_tid, NULL, compare_length_thread, &compare_eq_data);
    pthread_create(&swap_tid1, NULL, swap_thread, &swap_data1);
    pthread_create(&swap_tid2, NULL, swap_thread, &swap_data2);
    pthread_create(&swap_tid3, NULL, swap_thread, &swap_data3);
    pthread_create(&monitor, NULL, count_monitor, counters);

    pthread_join(compare_asc_tid, NULL);
    pthread_join(compare_desc_tid, NULL);
    pthread_join(compare_eq_tid, NULL);
    pthread_join(monitor, NULL);
    pthread_join(swap_tid1, NULL);
    pthread_join(swap_tid2, NULL);
    pthread_join(swap_tid3, NULL);

    return 0;
}
