#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#define N 1000
#define START (-100)
#define STOP (100)
#define ITERATIONS_COUNT 10000

double * generate_double_arr(size_t n){
    return (double *) malloc(n * sizeof(double));
}


double *fill_vector(){
    double * res = generate_double_arr(N);
    for(size_t i = 0; i < N; i++){
        res[i] = i*i;
    }
    return res;
}

void fill_vector_with_value(double *res, double value) {
    for (size_t i = 0; i < N; i++) {
        res[i] = value;
    }
}

double* generate_matrix_a() {
    double *res = generate_double_arr(N*N);
    for (size_t i = 0; i < N; i++) {
        for(size_t j = 0; j < N; j++){
            res[j*N +i] = (i == j ? (double)i : N);
        }
    }

    return res;
}


int main(int argc, char* argv[]) {
    double start;
    double end;
    start = omp_get_wtime();

    double *a = generate_matrix_a();

    double *x = generate_double_arr(N);
    fill_vector_with_value(x, 0);
    double *b = fill_vector();
    double *y = generate_double_arr(N);

    double norm_b_square = 0;
    double norm_y_square = 0;
    double right_part = 0;
    double *vec_rec = generate_double_arr(N);

    double tau = 0;

    double left_part = 0;
    double denominator;
    double numerator;

    size_t k;
    #pragma omp parallel shared(denominator,a, x, b, norm_b_square, right_part, y, vec_rec, numerator, tau, norm_y_square), private(k) default(none)
    {

        #pragma omp for reduction(+:norm_b_square)
        for (size_t i = 0; i < N; i++) {
            norm_b_square += b[i]*b[i];
        }

        #pragma omp single
        {
            double epsilon = pow(10, -5);
            right_part = epsilon * epsilon * norm_b_square;
            printf("Right part %f\n", right_part);
        }

        #pragma omp barrier

        #pragma omp for
        for (size_t i = 0; i < N; i++) {
            y[i] = 0.0;
            for (size_t j = 0; j < N; j++) {
                y[i] += a[i*N+j] * x[j];
            }
            y[i] -= b[i];
        }


        for(k = 0; k < ITERATIONS_COUNT; k++){

            #pragma omp single
            {
                denominator = 0;
                numerator = 0;
                norm_y_square = 0;
            }
            
            #pragma omp for
            for (size_t i = 0; i < N; i++) {
                double part = 0;
                for (size_t j = 0; j < N; j++) {
                    part += a[i * N + j] * y[j];
                }
                vec_rec[i] = part;
                part = pow(part, 2);
                
                #pragma omp atomic
                denominator += part;

            }



            #pragma omp for reduction(+:numerator)
            for (size_t i = 0; i < N; i++) {
                numerator += vec_rec[i]*y[i];
            }

            #pragma omp single
            tau =  numerator / denominator;
         

            #pragma omp for
            for(size_t j = 0; j < N; j++){
                x[j] = x[j] - tau*y[j];
            }


            #pragma omp for
            for (size_t i = 0; i < N; i++) {
                y[i] = 0.0;
                for (size_t j = 0; j < N; j++) {
                    y[i] += a[i*N+j] * x[j];
                }
                y[i] -= b[i];
            }


            #pragma omp for reduction(+:norm_y_square)
            for (size_t i = 0; i < N; i++) {
                norm_y_square += y[i]*y[i];
            }

            if(norm_y_square < right_part){
                if(omp_get_thread_num() == 0){
                    printf("Finished on: %zu\n", k+1);
                }
                break;
            }

            #pragma omp barrier

        }



    }


    free(a);
    free(b);
    free(x);
    free(y);
    free(vec_rec);
    end = omp_get_wtime();
    printf("Work took %f seconds. Relative time: %f\n", end - start, (end-start)/k);

}

