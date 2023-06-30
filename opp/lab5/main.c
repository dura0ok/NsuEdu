#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <mpi.h>
#include <math.h>

#define MAX_TASKS 15000
#define ITERATIONS_COUNT 1000
#define L 2
#define REQUEST_TASK 228
#define STOP_CODE 229
#define TO_RECEIVER 1
#define SEND_WORK_TAG 2

int *tasks;
int size;
int rank;
int offset;
double *times;
pthread_mutex_t mutex;

struct Job_Requester{
    int count_tasks;
    int local_res;
};

struct Job_Requester do_job(int length) {
    int res = 0;
    int count_tasks = 0;
    struct Job_Requester req;
    while (1) {
        pthread_mutex_lock(&mutex);
        if (offset >= length) {
            pthread_mutex_unlock(&mutex);
            break;
        }
        int current_offset = offset++;
        pthread_mutex_unlock(&mutex);

        int weight = tasks[current_offset];
//        }
        for (int j = 0; j < weight; j++) {
            res += (int)sqrt(j);
        }
        count_tasks++;
    }
    req.local_res = res;
    req.count_tasks = count_tasks;
    return req;
}

void fill_tasks(int iter_count) {
    for (int i = 0; i < size * MAX_TASKS; i++) {
        tasks[i] = abs(50 - i % 100) * abs(rank - iter_count % size) * L;
    }
}

struct Job_Requester do_request_tasks() {
    for (int i = 0; i < size; i++) {
        if (i == rank) continue;
        int req_code = REQUEST_TASK;
        int help_length;
        //

        MPI_Send(&req_code, 1, MPI_INT, i, TO_RECEIVER, MPI_COMM_WORLD);
        MPI_Recv(&help_length, 1, MPI_INT, i, SEND_WORK_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        if (help_length > 0) {

            MPI_Recv(tasks, help_length, MPI_INT, i, SEND_WORK_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);


            pthread_mutex_lock(&mutex);
            offset = 0;
            pthread_mutex_unlock(&mutex);
        }

        return do_job(help_length);
    }
}

void find_max_min(const double* arr, int len, double* max, double* min) {
    // Initialize max and min to the first element of the array
    *max = *min = arr[0];

    // Iterate over the array and update max and min as needed
    for (int i = 1; i < len; i++) {
        if (arr[i] > *max) {
            *max = arr[i];
        }
        if (arr[i] < *min) {
            *min = arr[i];
        }
    }
}



void do_tasks() {
    double start_time = MPI_Wtime();
    struct Job_Requester res1 = do_job(size * MAX_TASKS);
    struct Job_Requester res2 = do_request_tasks();
    double end_time = MPI_Wtime();

    int general_do_task_counter = res1.count_tasks + res2.count_tasks;
    int general_proc_local_res = res1.local_res + res2.local_res;
    int general_proc_global_res = 0;
    MPI_Allreduce(&general_proc_local_res, &general_proc_global_res, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
    double work_time = end_time - start_time;
    printf("Proc %d | Tasks done %d || Global res %d || Time %f\n", rank, general_do_task_counter, general_proc_global_res, work_time);
    MPI_Gather(&work_time, 1, MPI_DOUBLE, times, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    if(rank == 0){
        double max_time;
        double min_time;
        find_max_min(times, size, &max_time, &min_time);
        double imbalance_time = max_time - min_time;
        printf("IMBALANCE TIME %f | Proportion %f\n", imbalance_time, imbalance_time / max_time * 100);
    }


}



void* worker_func(void* arg) {
    for (int i = 0; i < ITERATIONS_COUNT; i++) {
        pthread_mutex_lock(&mutex);
        offset = 0;
        fill_tasks(i);
        pthread_mutex_unlock(&mutex);
        do_tasks();
    }
    int stop_code = STOP_CODE;
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Send(&stop_code, 1, MPI_INT, rank, TO_RECEIVER, MPI_COMM_WORLD);
    return NULL;
}

void* receiver_func(void* arg) {
    while (1) {
        int req_code_buf;
        MPI_Status status_worker_requester;

        MPI_Recv(&req_code_buf, 1, MPI_INT, MPI_ANY_SOURCE, TO_RECEIVER, MPI_COMM_WORLD, &status_worker_requester);

        if (req_code_buf == STOP_CODE){
            break;
        }

        size_t length = size * MAX_TASKS;
        int new_offset = offset + (int)((int)(length - offset) * 0.1);
        int tasks_length = new_offset - offset;

        MPI_Send(&tasks_length, 1, MPI_INT, status_worker_requester.MPI_SOURCE, SEND_WORK_TAG, MPI_COMM_WORLD);

        if (tasks_length > 0 ) {
            pthread_mutex_lock(&mutex);
            int old_offset = offset;
            offset = new_offset;
            pthread_mutex_unlock(&mutex);
            MPI_Send(&tasks[old_offset], tasks_length, MPI_INT, status_worker_requester.MPI_SOURCE, SEND_WORK_TAG, MPI_COMM_WORLD);
        }
    }
    return NULL;
}

int main(int argc, char* argv[]) {
    int provided;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
    double start_time = MPI_Wtime();
    if (provided != MPI_THREAD_MULTIPLE) {
        printf("Invalid thread level support\n");
        exit(-1);
    }

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    tasks = (int*) malloc(size * MAX_TASKS * sizeof(int));
    if(rank == 0){
        times =(double*) malloc(size * sizeof(double));
    }

    pthread_t worker, receiver;
    pthread_mutex_init(&mutex, NULL);
    pthread_create(&worker, NULL, worker_func, NULL);
    pthread_create(&receiver, NULL, receiver_func, NULL);

    pthread_join(worker, NULL);
    pthread_join(receiver, NULL);

    if (tasks != NULL)
        free(tasks);
    if (rank == 0)
        free(times);

    double end_time = MPI_Wtime();
    printf("Time: %f\n", end_time - start_time);
    MPI_Finalize();
    return 0;
}