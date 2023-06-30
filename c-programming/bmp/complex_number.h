#pragma once
#include "math.h"

struct T_complex_number{
    double real;
    double complex;
};

void square(struct T_complex_number *complex_num);
double magnitude(struct T_complex_number complex_num);
void add(struct T_complex_number *current, struct T_complex_number addition);