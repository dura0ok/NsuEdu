#pragma once
#include "stdio.h"


struct Converter{
    char * from;
    char * to;
    double (*func)(double);
};



struct Converter* find_to_main_converter(char from[BUFSIZ]);
struct Converter* find_from_main_converter(char to[BUFSIZ]);
