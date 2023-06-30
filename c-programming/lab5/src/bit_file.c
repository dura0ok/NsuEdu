#include <malloc.h>
#include <limits.h>
#include <assert.h>
#include <stdbool.h>
#include "bit_file.h"

T_bit_file *bf_create(FILE *fp) {


    T_bit_file *bf = malloc(sizeof(T_bit_file));
    if (!bf) {
        assert("error");
        fclose(fp);
        return NULL;
    }

    bf->fp = fp;

    bf->read_char = getc(fp);
    bf->bit_counter = 0;

    return bf;
}


void bf_close(T_bit_file *bf) {
    if (!bf)
        return;

    fclose(bf->fp);
    free(bf);
}

int f_get_bit(T_bit_file *bf) {
    if (!bf) {
        assert("something went wrong or file ended");
        return -1;
    }

    if (bf->bit_counter >= CHAR_BIT) {
        bf->read_char = getc(bf->fp);
        bf->bit_counter = 0;
    }

    if (EOF == bf->read_char)
        return EOF;


    int bit = (int) READ_BIT(bf->read_char, bf->bit_counter);
    bf->bit_counter++;


    return bit;
}

