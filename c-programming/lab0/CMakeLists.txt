cmake_minimum_required(VERSION 3.5.1)
project(lab0 C)

set(SRC src/main.c src/linked_list.c)

add_executable(${PROJECT_NAME} ${SRC})
target_link_libraries(${PROJECT_NAME} m)

