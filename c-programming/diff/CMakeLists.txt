cmake_minimum_required(VERSION 3.16)
project(diff C)

set(CMAKE_C_STANDARD 99)

add_executable(diff main.c t_file.c diff.c smart_buffer.c)
