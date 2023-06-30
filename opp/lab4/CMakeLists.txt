cmake_minimum_required(VERSION 3.25)
project(lab4 C)

set(CMAKE_C_STANDARD 23)

add_executable(lab4 speed2.c)

find_package(MPI REQUIRED)
set(CMAKE_CXX_COMPILE_FLAGS ${CMAKE_CXX_COMPILE_FLAGS} ${MPI_COMPILE_FLAGS})
set(CMAKE_CXX_LINK_FLAGS ${CMAKE_CXX_LINK_FLAGS} ${MPI_LINK_FLAGS})
include_directories(${MPI_INCLUDE_PATH})


target_link_libraries(lab4 ${MPI_LIBRARIES})