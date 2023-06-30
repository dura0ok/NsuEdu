#include "t_file.h"
#include "stdio.h"
#include "assert.h"

struct T_file open_file(const char* file_path){
    struct T_file new_file = {.file_path = file_path};
    FILE *file= fopen(file_path, "rb");
    assert(file != NULL);
    new_file.file = file;

    return new_file;
}

void close_file(struct T_file t_file){
    assert(fclose(t_file.file) == 0);
}

long get_file_size(struct T_file t_file){
    assert(fseek(t_file.file, 0, SEEK_END) == 0);  // seek to end of file
    long size = ftell(t_file.file); // get current file pointer
    assert(fseek(t_file.file, 0, SEEK_SET) == 0); // seek back to beginning of file
    return size;
}

