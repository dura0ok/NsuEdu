#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

#define N 1000
#define START (-100)
#define STOP (100)
#define ITERATIONS_COUNT 10000

double * generate_double_arr(size_t n){
    return (double *) malloc(n * sizeof(double));
}

double generate_random_in_range(double min, double max) {
    double range = (max - min);
    double div = RAND_MAX / range;
    return min + (rand() / div);
}

double *generate_vector_with_filled_value(double value){
    double * res = generate_double_arr(N);
    for(size_t i = 0; i < N; i++){
        res[i] = value;
    }
    return res;
}

double *fill_vector(){
    double * res = generate_double_arr(N);
    for(size_t i = 0; i < N; i++){
        res[i] = i*i;
    }
    return res;
}

double *generate_vector_with_random_value(){
    double * res = generate_double_arr(N);
    for(size_t i = 0; i < N; i++){
        res[i] = generate_random_in_range(START, STOP);
    }
    return res;
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



double scalar_dot(const double *vec1, const double *vec2, size_t size){
    double res = 0;
    for(size_t i = 0; i < size; i++){
        res += vec1[i]*vec2[i];
    }
    return res;
}

void multiple_vec_with_num(const double *vec, double num, double *res, size_t size){
    for(size_t i = 0; i < size; i++){
        res[i] = vec[i] * num;
    }
}

double calculate_squared_norm(double *vec, size_t size){
    double res = 0;
    for(size_t i = 0; i < size; i++){
        res += (double)powl(vec[i], 2);
    }
    return res;
}


bool check_convergence(double *left, double right_part){
    return  calculate_squared_norm(left, N) < right_part;
}

double calculate_right_part(double *b){
    double epsilon = powl(10, -5);
    double a = calculate_squared_norm(b, N);
    return a * epsilon * epsilon;
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

void calculate_vector_y(const double *matrix_a, double* x, double *b, double *res) {
    multiple_matrix_and_vector(matrix_a, x, res);
    subtract_vectors(res, b, res, N);
}

double *calculate_ay_n(double *a, double *y) {
    double *res = generate_double_arr(N);
    multiple_matrix_and_vector(a,y, res);
    return res;
}

double tau_update(double *a, double *y) {
    double *ay_n = calculate_ay_n(a, y);
    double numerator = scalar_dot(y, ay_n, N);
    double denumerator = scalar_dot(ay_n, ay_n, N);
    return numerator / denumerator;
}

void save_matrix_to_file(double* matrix, char* filename) {
    // Open the file for writing
    FILE* file = fopen(filename, "w");

    // Write the matrix dimensions to the file
    fprintf(file, "%d ", N);

    // Write the matrix elements to the file
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            fprintf(file, "%f ", matrix[i*N + j]);
        }
        //fprintf(file, "\n");
    }

    // Close the file
    fclose(file);
}

void save_vector_to_file(const char* filename, double * vector) {
    FILE* fp = fopen(filename, "w");
    if (fp == NULL) {
        printf("Error opening file %s\n", filename);
        return;
    }

    fprintf(fp, "%d ", N);

    for (int i = 0; i < N; i++) {
        fprintf(fp, "%f\n", vector[i]);
    }

    fclose(fp);
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

int main() {
    srand (time ( NULL));
    double* a = generate_matrix_a();
    double* b = fill_vector();

    double right_part = calculate_right_part(b);
    //printf("Right part %f\n", right_part);

    double* x = generate_vector_with_filled_value(0);
    double *y = generate_double_arr(N);
    double *tau_y = generate_double_arr(N);


//    print_matrix(a, N);
//    print_vec(b, N);

    calculate_vector_y(a, x, b, y);
    for(size_t i = 0; i < ITERATIONS_COUNT; i++){
        //printf("%zu\n", i+1);

        double tau = tau_update(a, y);
        //printf("Tau %f\n", tau);

        multiple_vec_with_num(y, tau, tau_y, N);
        subtract_vectors(x, tau_y, x, N);

        //print_vec(x, N);
        calculate_vector_y(a, x, b, y);
        if(check_convergence(y, right_part)){
            printf("Finished on: %zu\n", i+1);
//            if(i+1 > 1500){
//                save_matrix_to_file(a, "matrix.txt");
//                save_vector_to_file("vector.txt", b);
//            }
            break;
        }

    }

    //print_vec(x, N);
    free(a);
    free(b);
    free(x);
        free(y);
        free(tau_y);
    return 0;
}
