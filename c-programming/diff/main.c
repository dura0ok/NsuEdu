#include <stdio.h>
#include "t_file.h"
#include "diff.h"

void close_data(struct T_file first, struct T_file second){
    close_file(first);
    close_file(second);
}

int main(int argc, char *argv[]) {
    if(argc < 3) return 0;

    struct T_file first = open_file(argv[1]);
    struct T_file second = open_file(argv[2]);
    long first_size = get_file_size(first);
    long second_size = get_file_size(second);

    if(first_size != second_size){
        printf("Size mismatch: file1 is %ld bytes, file 2 is %ld bytes\n", first_size, second_size);
    }

    compare_files(first, second);

    close_data(first, second);
    return 0;
}
