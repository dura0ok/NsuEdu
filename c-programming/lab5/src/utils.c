#include "utils.h"

void dump_data(FILE *file, unsigned char c) {
    putc(c, file);
}


void set_bit(struct T_smart_byte *bit, unsigned char value, FILE *file) {
    if (bit->busy_size == 8) {
        bit->busy_size = 0;
        if (file != NULL) {
            dump_data(file, bit->data);
        }
    }

    unsigned char mask = 1 << (7 - bit->busy_size);
    bit->busy_size++;
    if (value == 0) {
        mask = ~mask;
        bit->data = bit->data & mask;
    } else {
        bit->data = bit->data | mask;
    }
}

void pull_bit(struct T_smart_byte *bit, FILE *file) {
    while (bit->busy_size != 8) {
        set_bit(bit, 0, file);
    }
    dump_data(file, bit->data);
    bit->busy_size = 0;
}




