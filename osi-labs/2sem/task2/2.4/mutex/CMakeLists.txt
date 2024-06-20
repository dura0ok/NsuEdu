cmake_minimum_required(VERSION 3.17.3)
project(mutex C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(mutex
        main.c
        mutex.c
        queue.c)
