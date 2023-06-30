cmake_minimum_required(VERSION 3.5.1)
project(lab8-0 C)

set(SRC src/main.c src/kruskal.c)

add_executable(${PROJECT_NAME} ${SRC})

