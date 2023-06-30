#include <stdio.h>
#include <mpi/mpi.h>
#include <stdlib.h>
#include <time.h>

#define ROWS_A 4
#define ROWS_B 4
#define COLS_A ROWS_B
#define COLS_B 4

#define X 1
#define Y 0

#define DIMENSION 2

void part_multiply(double *c, const double *a, const double *b, int sizeRow, int sizeColumn) {
    for (int i = 0; i < sizeRow; ++i) {
        for (int j = 0; j < sizeColumn; ++j) {
            c[i * sizeColumn + j] = 0;
            for (int k = 0; k < ROWS_B; ++k) {
                c[i * sizeColumn + j] += a[i * ROWS_B + k] * b[k * sizeColumn + j];
            }
        }
    }
}


double * generate_double_arr(size_t n){
    return (double *) malloc(n * sizeof(double));
}

double generate_random_in_range(double min, double max) {
    double range = (max - min);
    double div = RAND_MAX / range;
    return min + (rand() / div);
}

double fill_matrix_random_values(double *matrix, size_t rows, size_t cols){
    for(size_t i = 0; i < rows; i++){
        for(size_t j = 0; j < cols; j++){
            //matrix[i*cols + j] = generate_random_in_range(10, 100);
            matrix[i*cols + j] = (double)((i) * cols + j);
        }
    }
}

void print_matrix(double *arr, size_t rows, size_t cols){
    for(size_t i = 0; i < rows; i++){
        for(size_t j = 0; j < cols; j++){
            printf("%lf ", arr[i*cols + j]);
            //printf("%zu\n", i*rows + j);
        }
        printf("\n");
    }
    printf("\n\n");
}

int main(int argc, char** argv) {
    srand (time(NULL));
    int rank, size;
    double *a, *b, *c;

    MPI_Init(NULL, NULL);
    double start_time = MPI_Wtime();
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int dims[DIMENSION]={0,0}, periods[DIMENSION]={0,0};

    // fill array dims dims[0] * dims[1] = size
    // (nnodes, ndims, dims) 
    MPI_Dims_create(size,DIMENSION, dims);
    MPI_Comm custom_communicator;
    
    // 0 reorder(bool)
    MPI_Cart_create(MPI_COMM_WORLD, DIMENSION, dims, periods, 0, &custom_communicator);
    int custom_rank;
    MPI_Comm_rank(custom_communicator, &custom_rank);

    int coords[DIMENSION];
    // rank to coords
    MPI_Cart_coords(custom_communicator, rank, DIMENSION, coords);
    //printf("Coords %d %d\n", coords[0], coords[1]);


    if(coords[Y] == 0 && coords[X] == 0){
        //printf("Num procs: %d\n", size);
        a = generate_double_arr(ROWS_A*COLS_A);
        fill_matrix_random_values(a, ROWS_A, COLS_A);
        b = generate_double_arr(ROWS_B*COLS_B);
        fill_matrix_random_values(b, ROWS_B, COLS_B);

        c = generate_double_arr(ROWS_A*COLS_B);
//        printf("Matrix a\n");
//        print_matrix(a, ROWS_A, COLS_A);
//        printf("Matrix b\n");
//        print_matrix(b, ROWS_B, COLS_B);
    }


    int numberRows = ROWS_A / dims[Y];
    int numberColumns = COLS_B / dims[X];


    double *partA = generate_double_arr(numberRows * COLS_A);
    double *partB = generate_double_arr(numberColumns * ROWS_B);
    double *partC = generate_double_arr(numberColumns * numberRows);

    int rowsFlags[2]={0, 1};
    MPI_Comm rows;
    MPI_Cart_sub(custom_communicator, rowsFlags, &rows);

    int colsFlags[2]={1, 0};
    MPI_Comm cols;
    MPI_Cart_sub(custom_communicator, colsFlags, &cols);


    if(coords[X] == 0){
        MPI_Scatter(a, numberRows * COLS_A, MPI_DOUBLE, partA, numberRows * COLS_A, MPI_DOUBLE, 0, cols);
//        if(rank == 0){
//            printf("Number of rows %d, %d %d\n", numberRows, dims[0], dims[1]);
//            print_matrix(partA, numberRows, COLS_A);
//        }

    }
    MPI_Bcast(partA, numberRows * COLS_A, MPI_DOUBLE, 0, rows);
//    if(rank == 3){
//        print_matrix(partA, numberRows, COLS_A);
//    }
    
    MPI_Datatype columns;
    MPI_Datatype resized_columns;
    /*
    int MPIAPI MPI_Type_vector(
        int          count,
        int          blocklength,
        int          stride,
        MPI_Datatype oldtype,
      _Out_ MPI_Datatype *newtype
    );
    */
    MPI_Type_vector(ROWS_B, numberColumns, COLS_B, MPI_DOUBLE, &columns);
    MPI_Type_commitint MPI_Type_create_resized(MPI_Datatype oldtype,
                            MPI_Aint lb, MPI_Aint extent, MPI_Datatype * newtype)(&columns);
    /*

        int MPI_Type_create_resized(MPI_Datatype oldtype,
                            MPI_Aint lb, MPI_Aint extent, MPI_Datatype * newtype)

    */

    MPI_Type_create_resized(columns, 0, (size_t)(numberColumns * sizeof(double)), &resized_columns);
    MPI_Type_commit(&resized_columns);
    if(coords[Y] == 0){
        MPI_Scatter(b, 1, resized_columns, partB, numberColumns * ROWS_B, MPI_DOUBLE, 0, rows);
    }
//    if(rank == 1){
//        print_matrix(partB, ROWS_B, numberColumns);
//    }

    MPI_Bcast(partB, numberColumns * ROWS_B, MPI_DOUBLE, 0, cols);
//    if(rank == 3){
//        print_matrix(partB, ROWS_B, numberColumns);
//    }

    part_multiply(partC, partA, partB, numberRows, numberColumns);

//    if(rank == 0){
//        printf("Matrix part c\n");
//        print_matrix(partC, numberRows, numberColumns);
//        printf("====\n");
//    }

    MPI_Datatype block;
    MPI_Datatype block_resized;
    MPI_Type_vector(numberRows, numberColumns, COLS_B, MPI_DOUBLE, &block);
    MPI_Type_commit(&block);
    MPI_Type_create_resized(block, 0, (size_t)(numberColumns * sizeof(double)), &block_resized);
    MPI_Type_commit(&block_resized);
    int recv_counts[size];
    for(size_t i = 0; i < size; i++){
        recv_counts[i] = 1;
    }

    int displs[size];

    for (int i = 0; i < dims[Y]; ++i) {
        for (int j = 0; j < dims[X]; ++j) {
            displs[i * dims[X] + j] = i * dims[X] * numberRows + j;
        }
    }
        
    if(rank == 0){
         for(int i = 0; i < size; i++){
            printf("%d - %d\n", i, displs[i]);    
        }
    }
    
    /*
    
    MPI_Gatherv(
  _In_      void         *sendbuf,
            int          sendcount,
            MPI_Datatype sendtype,
  _Out_opt_ void         *recvbuf,
  _In_opt_  int          *recvcounts[],
  _In_opt_  int          *displs[],
            MPI_Datatype recvtype,
            int          root,
            MPI_Comm     comm
);
    */

    MPI_Gatherv(partC, numberColumns*numberRows, MPI_DOUBLE, c, recv_counts, displs, block_resized, 0, custom_communicator);


    if(custom_rank == 0){
        //print_matrix(c, ROWS_A, COLS_B);
        free(a);
        free(b);
        free(c);
    }

    free(partA);
    free(partB);
    free(partC);

    MPI_Type_free(&columns);
    MPI_Type_free(&resized_columns);
    MPI_Type_free(&block);
    MPI_Type_free(&block_resized);
    double end_time = MPI_Wtime();
//    if(rank == 0){
//        printf("%f\n", end_time-start_time);
//    }
    MPI_Finalize();
}
