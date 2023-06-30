#include <iostream>
#include "mpi.h"
#include <random>
#include <cassert>

using namespace std;

//#define VECTOR_SIZE 200000
#define VECTOR_SIZE 150000


inline int generateRandomInRange(int a, int b) {
    random_device rd;
    uniform_int_distribution gen(a, b);
    return gen(rd);
}

void fill_vector_values(int *vec) {
    for (size_t i = 0; i < VECTOR_SIZE; i++) {
        vec[i] = generateRandomInRange(1, 100);
    }

}


std::string print_vec(int *vec, int n) {
    std::string buffer;
    for (size_t i = 0; i < n; i++) {
        buffer.append(to_string(vec[i]));
        buffer.append(", ");
    }
    return buffer;
}

// функция, которая считает не параллельно, нужна для сверки результатов
int calculate(const int *vec1, const int *vec2) {
    int res = 0;
    for (int i = 0; i < VECTOR_SIZE; i++) {
        for (int j = 0; j < VECTOR_SIZE; j++) {
            res += vec1[i] * vec2[j];
        }
    }
    return res;
}


int main(int argc, char **argv) {
    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    size_t sub_size = VECTOR_SIZE / size;
    size_t rest = VECTOR_SIZE % size;
    int result = 0;

    int *vec1 = NULL;

    auto *received_vec1 = new int[sub_size];
    auto *vec2 = new int[VECTOR_SIZE];

    // когда вызывается scatter он сначало иницилизирует вектора входные, потом отправляет по другим потокам включая себя
    if (rank == 0) {
        vec1 = new int[VECTOR_SIZE];


        fill_vector_values(vec1);
        fill_vector_values(vec2);

//        std::cout << "generated data : vec1 : {" << print_vec(vec1, VECTOR_SIZE) << "}, vec2 : {"
//                  << print_vec(vec2, VECTOR_SIZE) << "}" << std::endl;
    }

    // send parts vec1 to processes(includes 0 root process)
    MPI_Scatter(vec1, sub_size, MPI_INT, received_vec1, sub_size, MPI_INT, 0, MPI_COMM_WORLD);
    //send vec2 to all processes
    MPI_Bcast(vec2, (int) VECTOR_SIZE, MPI_INT, 0, MPI_COMM_WORLD);

//    std::cout << "rank is : " << rank << ", left : {" << print_vec(received_vec1, sub_size) << "}, right : {"
//              << print_vec(vec2, VECTOR_SIZE) << "}" << std::endl;

    // calculate part of vec1 in some process
    int curr_res = 0;
    for (size_t i = 0; i < sub_size; i++) {
        for (size_t j = 0; j < VECTOR_SIZE; j++) {
            curr_res += received_vec1[i] * vec2[j];
        }
    }


    //summ all calculations
    MPI_Reduce(&curr_res, &result, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);


    if (rank == 0) {
        std::cout << "Result " << result << endl;
        //std::cout << "calculate " << calculate(vec1, vec2) << endl;
        delete[]vec1;

    }
    delete[] received_vec1;
    delete[] vec2;
    MPI_Finalize();

    return 0;
}
