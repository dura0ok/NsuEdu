cmake_minimum_required(VERSION 3.5.1)
project(lab9 C)

set(SRC src/main.c src/graph.c src/dijkstra.c src/smart_buffer.c src/utils.c)

add_executable(${PROJECT_NAME} ${SRC})

