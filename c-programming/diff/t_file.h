#pragma once
#include <bits/types/FILE.h>

struct T_file{
    const char* file_path;
    FILE * file;
};

struct T_file open_file(const char* file_path);
long get_file_size(struct T_file t_file);
void close_file(struct T_file t_file);

