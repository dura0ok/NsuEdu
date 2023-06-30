cmake_minimum_required(VERSION 3.5.1)
project(lab1-1 C)

set(SRC src/main.c src/queue.c)

add_executable(${PROJECT_NAME} ${SRC})

