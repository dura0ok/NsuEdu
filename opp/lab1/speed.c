#include <stdio.h>
#include <math.h>
#include <mpi.h>
#include <stdlib.h>
#include <string.h>

#define N 1000
#define START (-100)
#define STOP (100)
#define ITERATIONS_COUNT 10000

void calculate_right_part(int rank, const int *b_send_counts, const int *b_displs, const double *b, double *right_part) {
    double *start_b = (double *)malloc(b_send_counts[rank] * sizeof(double));
    MPI_Scatterv(b, b_send_counts, b_displs, MPI_DOUBLE, start_b, b_send_counts[rank], MPI_DOUBLE, 0, MPI_COMM_WORLD);
    size_t local_sum = 0;
    size_t global_sum = 0;
    for (int i = 0; i < b_send_counts[rank]; i++) {
        local_sum += start_b[i] * start_b[i];
    }
    MPI_Reduce(&local_sum, &global_sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    double epsilon = powl(10, -5);

    if(rank == 0){
        (*right_part) = global_sum * epsilon * epsilon;
    }
    MPI_Bcast(right_part, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    free(start_b);
}


double * generate_double_arr(size_t n){
    return (double *) malloc(n * sizeof(double));
}

int * generate_int_arr(size_t n){
    return (int *) malloc(n * sizeof(int));
}

double* read_matrix_from_file(const char* filename) {
    FILE* fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Error opening file %s\n", filename);
        return NULL;
    }

    size_t n;
    fscanf(fp, "%zd", &n);
    char c = fgetc(fp);

    // Allocate memory for the matrix
    double* arr = generate_double_arr(n*n);

    // Read the matrix elements from the file
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            double res;
            fscanf(fp, "%lf", &res);
            arr[i* n + j] = res;
            fgetc(fp);
        }
    }

    fclose(fp);
    return arr;
}

double* read_vector_from_file(const char* filename) {
    FILE* fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Error opening file %s\n", filename);
        return NULL;
    }

    size_t n;
    fscanf(fp, "%zd", &n);
    fgetc(fp);

    // Allocate memory for the matrix
    double* arr = generate_double_arr(n);

    // Read the matrix elements from the file
    for (int i = 0; i < n; i++) {
        double res;
        fscanf(fp, "%lf", &res);
        arr[i] = res;
        fgetc(fp);
    }

    fclose(fp);
    return arr;
}



void fill_vector_with_value(double *res, double value) {
    for (size_t i = 0; i < N; i++) {
        res[i] = value;
    }
}


double scalar_dot(double *row, double *column, size_t size){
    double res = 0;
    for(size_t i = 0; i < size; i++){
        res += row[i] * column[i];
    }
    return res;
}

void print_matrix(double *arr, size_t size){
    for(size_t i = 0; i < size; i++){
        for(size_t j = 0; j < size; j++){
            printf("%lf ", arr[i*size + j]);
        }
        printf("\n");
    }
    printf("\n\n");
}

void print_vec(double *vec, size_t size){
    for(size_t i = 0; i < size; i++){
        printf("%lf\n", vec[i]);
    }
    printf("\n\n");
}


void subtract_vectors(const double *vec1, const double *vec2, double *res, int size){
    for(size_t i = 0; i < (size_t)size; i++){
        res[i] = vec1[i] - vec2[i];
    }
}

void multiple_matrix_and_vector(const double *matrix, const double *vec, double *res){
    memset(res, 0, sizeof(double) * N);
    for(size_t i = 0; i < N; i++) {
        for (size_t j = 0; j < N; j++) {
            res[j] += matrix[j*N + i] * vec[i];
        }
    }

}


double* generate_matrix_a() {
    double *res = generate_double_arr(N*N);
    for (size_t i = 0; i < N; i++) {
        for(size_t j = 0; j < N; j++){
            //res[i]= generate_random_in_range(START, STOP);
            res[j*N +i] = (i == j ? i : N);
            // if(i == j){
            //     //res[i*N + j] += (N / 8.0);
            //     res[i*N + j] = (N*8);
            // }
        }
    }

    //    for (size_t i = 0; i < N; i++) {
    //        for(size_t j = 0; j < N; j++){
    //            if(i == j){
    //                res[i*N + j] =2;
    //            }else{
    //                res[i*N + j] =1;
    //            }
    //        }
    //    }



    return res;
}

double *fill_vector(){
    double * res = generate_double_arr(N);
    for(size_t i = 0; i < N; i++){
        res[i] = i*i;
    }
    return res;
}


void calculateY(int rank, const int *matrix_send_counts, const int *matrix_displs, const int *b_send_counts,
                const int *b_displs, double *x, double *y,
                double *matrix_rec_row, double *vec_rec1, double *vec_rec2) {


    int num_handle_rows = matrix_send_counts[rank] / N;


    for(size_t i = 0; i < num_handle_rows; i++){
        vec_rec2[i] =
                scalar_dot(&matrix_rec_row[i*N], x, N) - vec_rec1[i];
    }

    MPI_Allgatherv(vec_rec2, num_handle_rows, MPI_DOUBLE, y, b_send_counts,  b_displs, MPI_DOUBLE, MPI_COMM_WORLD);
}

int main(int argc, char ** argv){
    double start, end;
    int rank, size, n, rest;
    MPI_Init( & argc, & argv);
    start = MPI_Wtime();
    MPI_Comm_size(MPI_COMM_WORLD, & size);
    MPI_Comm_rank(MPI_COMM_WORLD, & rank);

    n = N / size;
    rest = N % size;

    int * matrix_send_counts = generate_int_arr(size);
    int *  matrix_displs = generate_int_arr(size);

    int *  b_send_counts = generate_int_arr(size);
    int * b_displs = generate_int_arr(size);


    int offset = 0;
    for (int i = 0; i < size; i++) {
        matrix_send_counts[i] = n * N;
        if (i < rest) {
            matrix_send_counts[i] += N;
        }
        matrix_displs[i] = offset;
        offset += matrix_send_counts[i];
    }



    offset = 0;
    for (int i = 0; i < size; i++) {
        b_send_counts[i] = n;
        if (i < rest) {
            b_send_counts[i] += 1;
        }
        b_displs[i] = offset;
        offset += b_send_counts[i];
    }

//
//        if (rank == 0) {
//            for (int i = 0; i < size; i++) {
//                printf("matrix_send_counts[%d] = %d\tdispls[%d] = %d\n", i, matrix_send_counts[i], i, matrix_displs[i]);
//            }
//            printf("\n");
//        }
//
//        if (rank == 0) {
//            for (int i = 0; i < size; i++) {
//                printf("b_send_counts[%d] = %d\tb_displs[%d] = %d\n", i, b_send_counts[i], i, b_displs[i]);
//            }
//            printf("\n");
//        }


    double *a, *b, *x, *y;
    //double *tau_y;
    x = generate_double_arr(N);
    fill_vector_with_value(x, 0);
    y = generate_double_arr(N);
    if(rank == 0){
        //        a = read_matrix_from_file("matrix.txt");
        //        b = read_vector_from_file("vector.txt");
        a = generate_matrix_a();
        b = fill_vector();




        //tau_y = generate_double_arr(N);
    }

    double right_part;
    calculate_right_part(rank, b_send_counts, b_displs, b, &right_part);

    if(rank == 0){
        printf("%f", right_part);
    }

    double *matrix_rec_row = generate_double_arr(matrix_send_counts[rank]);
    double *vec_rec1 = generate_double_arr(b_send_counts[rank]);
    double *vec_rec2 = generate_double_arr(b_send_counts[rank]);
    double *vec_rec3= generate_double_arr(b_send_counts[rank]);

    MPI_Scatterv(a, matrix_send_counts, matrix_displs, MPI_DOUBLE, matrix_rec_row, matrix_send_counts[rank], MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Scatterv(b, b_send_counts, b_displs, MPI_DOUBLE, vec_rec1, b_send_counts[rank], MPI_DOUBLE, 0, MPI_COMM_WORLD);

    calculateY(rank, matrix_send_counts, matrix_displs, b_send_counts, b_displs, x, y,
               matrix_rec_row, vec_rec1, vec_rec2);



    for(size_t i = 0; i < ITERATIONS_COUNT; i++){
        //double tau = tau_update(a, y);
        //double *ay_n = calculate_ay_n(a, y);
        // vec_rec2 = y
        // vec_rec1 ay part

        double local_ay_repeated_scalar_sum = 0;
        double global_ay_repeated_scalar_sum = 0;

        double local_ay_scalar_sum = 0;
        double global_ay_scalar_sum = 0;

        int num_handle_rows = matrix_send_counts[rank] / N;
        for(size_t j = 0; j < b_send_counts[rank]; j++) {
            //printf("%d %f\n", rank, matrix_rec_row[num_handle_rows*(i + 2)]);
            vec_rec3[j] =
                    scalar_dot(&matrix_rec_row[j * N], y, N); // ay
        }

        local_ay_scalar_sum += scalar_dot(vec_rec3, vec_rec3, b_send_counts[rank]);
        local_ay_repeated_scalar_sum += scalar_dot(vec_rec3, vec_rec2, b_send_counts[rank]); // числитель



        MPI_Reduce(&local_ay_scalar_sum, &global_ay_scalar_sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
        MPI_Reduce(&local_ay_repeated_scalar_sum, &global_ay_repeated_scalar_sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

        double tau;
        if(rank == 0){
            tau = global_ay_repeated_scalar_sum / global_ay_scalar_sum;
            if(i % 1000 == 0)
                printf("Tau: %f\n", tau);
        }

        MPI_Bcast(&tau, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        for(size_t j = 0; j < num_handle_rows; j++) {
            vec_rec3[j] = x[b_displs[rank]+j] - vec_rec2[j] * tau;
        }

        MPI_Gatherv(vec_rec3, num_handle_rows, MPI_DOUBLE, x, b_send_counts,  b_displs, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        MPI_Bcast(x,N,MPI_DOUBLE,0,MPI_COMM_WORLD);
        calculateY(rank, matrix_send_counts, matrix_displs, b_send_counts, b_displs, x, y,
                   matrix_rec_row, vec_rec1, vec_rec2);


        double local_left_scalar_y = 0;
        double global_left_scalar_y = 0;
        for(size_t j = 0; j < N; j++) {
            local_left_scalar_y += y[j]*y[j];
        }
        // MPI_Reduce(&local_left_scalar_y, &global_left_scalar_y, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

        //  MPI_Bcast(&global_left_scalar_y, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
//            MPI_Bcast(&right_part, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        if(local_left_scalar_y < right_part){
            if(rank == 0){
                printf("Finished on: %zu\n", i+1);
            }
            break;
        }
    }

//        if(rank == 0){
//            print_vec(x, 2);
//        }


    free(matrix_send_counts);
    free(b_send_counts);
    free(matrix_displs);
    free(b_displs);
    free(matrix_rec_row);
    free(vec_rec1);
    free(vec_rec2);
    free(vec_rec3);
    free(y);
    if(rank == 0){
        free(a);
        free(b);
        //free(tau_y);
    }

    if(rank == 0){
        end = MPI_Wtime();
        printf("Runtime = %f\n", end-start);
    }

    MPI_Finalize();
}

