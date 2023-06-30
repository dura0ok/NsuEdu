cmake_minimum_required(VERSION 3.21)
project(ttw C)

set(CMAKE_C_STANDARD 99)

add_executable(ttw main.c libbmp.c libbmp.h complex_number.c complex_number.h)
target_link_libraries(${PROJECT_NAME} m)
