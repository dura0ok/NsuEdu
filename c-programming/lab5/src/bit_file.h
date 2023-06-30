#pragma once

#include <stdio.h>

#define READ_BIT(A, B) ((A >> (7 - (B))) & 1)

typedef struct {
    FILE *fp;           //Disk file pointer
    int read_char;     //Current char read from stream
    int bit_counter;   //counter of the current bit
} T_bit_file;

T_bit_file *bf_create(FILE *fp);

void bf_close(T_bit_file *bf);

int f_get_bit(T_bit_file *bf);

