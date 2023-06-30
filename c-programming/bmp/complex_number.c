#include "complex_number.h"

void square(struct T_complex_number *complex_num){
    double temp = pow(complex_num->real, 2) - pow(complex_num->complex, 2);
    complex_num->complex = 2.0 * complex_num->real * complex_num->complex;
    complex_num->real = temp;
}

double magnitude(struct T_complex_number complex_num){
    return sqrt(pow(complex_num.real, 2) + pow(complex_num.complex, 2));
}

void add(struct T_complex_number *current, struct T_complex_number addition){
    current->real += addition.real;
    current->complex += addition.complex;
}