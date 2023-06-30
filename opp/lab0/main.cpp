#include <cstdio>
#include <random>
#include <iostream>

using namespace std;

inline int VECTOR_SIZE = 150000;

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

void print_vec(int *vec){
    for(size_t i = 0; i < VECTOR_SIZE; i++){
        std::cout << vec[i] << endl;
    }
    std::cout << "---------" << std::endl;
}


int calculate(const int *vec1, const int *vec2){
    int res = 0;
    for (int i = 0; i < VECTOR_SIZE; i++){
        for (int j = 0; j < VECTOR_SIZE; j++){
           // std::cout << i << " " << j << endl;
            res += vec1[i] * vec2[j];
        }
        //std::cout << std::endl;
    }
    return res;
}

int main(int argc, char** argv) {
    std::cout << VECTOR_SIZE << endl;
    auto *vec1 = new int[VECTOR_SIZE];
    auto *vec2 = new int[VECTOR_SIZE];
    fill_vector_values(vec1);
    fill_vector_values(vec2);
    auto res = calculate(vec1, vec2);
    std::cout << res;

    delete []vec1;
    delete []vec2;
}
