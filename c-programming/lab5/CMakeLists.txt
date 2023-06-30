cmake_minimum_required(VERSION 3.5.1)
project(lab5 C)

set(SRC src/main.c src/priority_queue.c src/codes_generator.c src/utils.c src/bit_file.c)

add_executable(${PROJECT_NAME} ${SRC})

