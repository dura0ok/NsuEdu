cmake_minimum_required(VERSION 3.5.1)
project(lab4 C)

set(SRC src/data_structures/stack.c src/main.c src/token.c src/token.h src/operator.c src/operator.h src/data_structures/linked_list.c src/data_structures/linked_list.h)

add_executable(${PROJECT_NAME} ${SRC})
target_link_libraries(${PROJECT_NAME} m)