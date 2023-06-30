cmake_minimum_required(VERSION 3.16.3)
project(cli C)

set(CMAKE_C_STANDARD 99)

add_executable(cli main.c converters.c converters.h)
