cmake_minimum_required(VERSION 3.24)
project(slau C)

set(CMAKE_C_STANDARD 11)

add_executable(slau speed.c)
target_link_libraries(${PROJECT_NAME} m)