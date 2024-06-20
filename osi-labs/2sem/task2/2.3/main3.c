#include <unistd.h>
#include "queue.h"
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <pthread.h>

#define STORAGE_CAPACITY 10
#define THREAD_COUNT 6
#define ASC 0
#define DESC 1
#define EQ 2
#define SWAP 3

void *compare_length_thread(void *data)
{
    ThreadData *thread_data = (ThreadData *)data;
    Storage *storage = thread_data->storage;
    int type = thread_data->type;

    while (1) {
        Node *curr1;

        if (pthread_rwlock_rdlock(&storage->sync) != 0) {
            perror("compare_length_thread: pthread_rwlock_rdlock failed");
            break;
        }

        if ((curr1 = storage->first) == NULL) {
            fprintf(stderr, "compare_length_thread: curr1 is NULL\n");
            pthread_rwlock_unlock(&storage->sync);
            break;
        }

        if (pthread_rwlock_rdlock(&curr1->sync) != 0) {
            perror("compare_length_thread: pthread_rwlock_rdlock failed");
            pthread_rwlock_unlock(&storage->sync);
            break;
        }

        pthread_rwlock_unlock(&storage->sync);

        Node *curr2 = curr1->next;
        while (curr2 != NULL) {

            if (pthread_rwlock_rdlock(&curr2->sync) != 0) {
                perror("compare_length_thread: pthread_rwlock_rdlock failed");
                break;
            }

            const int pair_count = strlen(curr1->value) - strlen(curr2->value);

            switch (type) {
            case EQ:
                if (pair_count == 0)
                    ++curr1->counter_eq;
                break;
            case ASC:
                if (pair_count < 0)
                    ++curr1->counter_asc;
                break;
            case DESC:
                if (pair_count > 0)
                    ++curr1->counter_dsc;
                break;
            }

            pthread_rwlock_unlock(&curr1->sync);
            curr1 = curr2;
            curr2 = curr1->next;
        }

        pthread_rwlock_unlock(&curr1->sync);
    }
    return NULL;
}

static void perform_swap(Node **curr1_next, Node *curr2, Node *curr3) {
    ++curr2->counter_swap;
    *curr1_next = curr3;
    curr2->next = curr3->next;
    curr3->next = curr2;
}

void *swap_thread(void *data)
{
    Storage *storage = (Storage *)data;

    while (1) {
        Node *curr1, *curr2, *curr3;

        if (pthread_rwlock_wrlock(&storage->sync) != 0) {
            perror("swap_thread: pthread_rwlock_wrlock failed");
            break;
        }

        if ((curr1 = storage->first) == NULL) {
            fprintf(stderr, "swap_thread: curr1 is NULL\n");
            pthread_rwlock_unlock(&storage->sync);
            break;
        }
        if (pthread_rwlock_wrlock(&curr1->sync) != 0) {
            perror("swap_thread: pthread_rwlock_wrlock failed");
            pthread_rwlock_unlock(&storage->sync);
            break;
        }

        if ((curr2 = curr1->next) == NULL) {
            fprintf(stderr, "swap_thread: curr2 is NULL\n");
            pthread_rwlock_unlock(&curr1->sync);
            pthread_rwlock_unlock(&storage->sync);
            break;
        }
        if (pthread_rwlock_wrlock(&curr2->sync) != 0) {
            perror("swap_thread: pthread_rwlock_wrlock failed");
            pthread_rwlock_unlock(&curr1->sync);
            pthread_rwlock_unlock(&storage->sync);
            break;
        }

        if ((rand() % 2) == 0) {
            perform_swap(&storage->first, curr1, curr2);
            curr1 = storage->first;
            curr2 = curr1->next;
        }

        pthread_rwlock_unlock(&storage->sync);

        curr3 = curr2->next;
        while (curr3 != NULL) {

            if (pthread_rwlock_wrlock(&curr3->sync) != 0) {
                perror("swap_thread: pthread_rwlock_wrlock failed");
                break;
            }

            if ((rand() % 2) == 0) {
                perform_swap(&curr1->next, curr2, curr3);
            }

            curr3 = curr1->next;
            pthread_rwlock_unlock(&curr1->sync);
            curr1 = curr3;
            curr2 = curr1->next;
            curr3 = curr2->next;
        }

        pthread_rwlock_unlock(&curr1->sync);
        pthread_rwlock_unlock(&curr2->sync);
    }
    return NULL;
}

void *count_monitor(void *arg) {
    Storage *storage = (Storage *)arg;

    while (1) {
        Node *curr1;

        if (pthread_rwlock_rdlock(&storage->sync) != 0) {
            perror("count_monitor: pthread_rwlock_rdlock failed");
            break;
        }

        if ((curr1 = storage->first) == NULL) {
            fprintf(stderr, "count_monitor: curr1 is NULL\n");
            pthread_rwlock_unlock(&storage->sync);
            break;
        }

        if (pthread_rwlock_rdlock(&curr1->sync) != 0) {
            perror("count_monitor: pthread_rwlock_rdlock failed");
            pthread_rwlock_unlock(&storage->sync);
            break;
        }

        pthread_rwlock_unlock(&storage->sync);

        printf("\t%s (swap: %d asc: %d dsc: %d eq: %d)\n",
               curr1->value, curr1->counter_swap,
               curr1->counter_asc, curr1->counter_dsc, curr1->counter_eq);

        int swap = curr1->counter_swap;
        int asc = curr1->counter_asc;
        int dsc = curr1->counter_dsc;
        int eq = curr1->counter_eq;

        Node *curr2 = curr1->next;
        while (curr2 != NULL) {

            if (pthread_rwlock_rdlock(&curr2->sync) != 0) {
                perror("count_monitor: pthread_rwlock_rdlock failed");
                break;
            }

            pthread_rwlock_unlock(&curr1->sync);

            printf("\t%s (swap: %d asc: %d dsc: %d eq: %d)\n",
                   curr2->value, curr2->counter_swap,
                   curr2->counter_asc, curr2->counter_dsc, curr2->counter_eq);

            swap += curr2->counter_swap;
            asc += curr2->counter_asc;
            dsc += curr2->counter_dsc;
            eq += curr2->counter_eq;

            curr1 = curr2;
            curr2 = curr1->next;
        }

        pthread_rwlock_unlock(&curr1->sync);
        printf("Total: %d asc: %d dsc: %d eq: %d\n", swap, asc, dsc, eq);

    }
    return NULL;
}

int main() {
    srand(time(NULL));
    Storage *storage = init_storage(STORAGE_CAPACITY);
    fill_storage(storage);
    print_storage(storage);

    pthread_t compare_asc_tid, compare_desc_tid, compare_eq_tid, swap_tid1, swap_tid2, swap_tid3, monitor;

    ThreadData compare_asc_data = {storage, ASC};
    ThreadData compare_desc_data = {storage, DESC};
    ThreadData compare_eq_data = {storage, EQ};

    pthread_create(&monitor, NULL, count_monitor, storage);
    pthread_create(&compare_asc_tid, NULL, compare_length_thread, &compare_asc_data);
    pthread_create(&compare_desc_tid, NULL, compare_length_thread, &compare_desc_data);
    pthread_create(&compare_eq_tid, NULL, compare_length_thread, &compare_eq_data);
    pthread_create(&swap_tid1, NULL, swap_thread, storage);
    pthread_create(&swap_tid2, NULL, swap_thread, storage);
    pthread_create(&swap_tid3, NULL, swap_thread, storage);

    pthread_join(compare_asc_tid, NULL);
    pthread_join(compare_desc_tid, NULL);
    pthread_join(compare_eq_tid, NULL);
    pthread_join(swap_tid1, NULL);
    pthread_join(swap_tid2, NULL);
    pthread_join(swap_tid3, NULL);
    pthread_join(monitor, NULL);

    return 0;
}
