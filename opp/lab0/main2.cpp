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

void fill_vector_values(int *vec){
    for(size_t i = 0; i < VECTOR_SIZE; i++){
        vec[i] = generateRandomInRange(1, 100);
    }

}


void print_vec(int *vec, int n){
    for(size_t i = 0; i < n; i++){
        std::cout << vec[i] << endl;
    }

}


int calculate(const int *vec1, const int *vec2){
    int res = 0;
    for (int i = 0; i < VECTOR_SIZE; i++){
        for (int j = 0; j < VECTOR_SIZE; j++){
            res += vec1[i] * vec2[j];
        }
    }
    return res;
}



int main(int argc, char **argv){
    int rank, size;
    int VEC_SIZE = VECTOR_SIZE;


    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    size_t sub_size = VEC_SIZE / size;
    size_t rest = VEC_SIZE % size;

    
    int result = 0;

    auto* received_vec1 = new int[sub_size];
    auto* received_vec2  = new int[VECTOR_SIZE];

    if(rank == 0){
        std::cout << sub_size << " " << rest << endl;

        auto *vec1 = new int[VECTOR_SIZE];
        auto *vec2 = new int[VECTOR_SIZE];


        fill_vector_values(vec1);
        fill_vector_values(vec2);

        for(size_t i = 1; i < size; i++) {
            //std::cout << sub_size*i << endl;
            MPI_Send(&vec1[sub_size*i], int(sub_size), MPI_INT, (int)i, (int)i, MPI_COMM_WORLD);
            MPI_Send(vec2, int(VEC_SIZE), MPI_INT, (int)i, (int)i, MPI_COMM_WORLD);

        }

    

        for(size_t i = 0; i < rest; i++){
          
            //std::cout << sub_size*size + i << endl; 
            if(i == 0){
                for(size_t j = 0; j < VECTOR_SIZE; j++){
                    result += vec1[sub_size*size + i]*vec2[j];
                }
                continue;
            }

            MPI_Send(&vec1[sub_size*size + i], 1, MPI_INT, (int)i, (int)i, MPI_COMM_WORLD);
            MPI_Send(vec2, VEC_SIZE, MPI_INT, (int)i, (int)i, MPI_COMM_WORLD);
        }


         for(size_t i = 0; i < sub_size; i++){
            int fixed_el = vec1[i];
            //std::cout << i << endl;
            for(size_t j = 0; j < VECTOR_SIZE; j++){
                result += fixed_el*vec2[j];
            }
        }


         

        for(size_t i = 1; i < size; i++){
        
            int local_res;
            MPI_Recv(&local_res, 1, MPI_INT, MPI_ANY_SOURCE, (int)i, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            result += local_res;
        
        }

         


         std::cout << "Result " << result << endl;
         //std::cout << "calculate " << calculate(vec1, vec2) << endl;

        delete []vec1;
        delete []vec2;
    }



    if(rank != 0){
        MPI_Recv(received_vec1, (int)sub_size, MPI_INT, 0, rank, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(received_vec2, VEC_SIZE, MPI_INT, 0, rank, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
   
        int curr_res = 0;
        for(size_t i = 0; i < sub_size; i++){
            int fixed_el = received_vec1[i];
            for(size_t j = 0; j < VECTOR_SIZE; j++){
                curr_res += fixed_el*received_vec2[j];
            }
        }


        if(rank + 1 <= rest){
            MPI_Recv(received_vec1, (int)sub_size, MPI_INT, 0, rank, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(received_vec2, VEC_SIZE, MPI_INT, 0, rank, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    
            for(size_t i = 0; i < sub_size; i++){
            int fixed_el = received_vec1[i];
            for(size_t j = 0; j < VECTOR_SIZE; j++){
                curr_res += fixed_el*received_vec2[j];
            }
            }
        }

        MPI_Send(&curr_res, 1, MPI_INT, 0, rank, MPI_COMM_WORLD);

    }
    

    delete []received_vec1;
    delete []received_vec2;
    MPI_Finalize();

    return 0;
}
