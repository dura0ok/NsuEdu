#pragma once

#include <stdio.h>

struct T_smart_byte {
    unsigned char data;
    unsigned char busy_size;
};


void set_bit(struct T_smart_byte *bit, unsigned char value, FILE *file);

void pull_bit(struct T_smart_byte *bit, FILE *file);





