cmake_minimum_required(VERSION 3.23)
project(evmlab)

set(CMAKE_CXX_STANDARD 17)

add_executable(evmlab simd.cpp)

find_library(blas libblas.a /usr/local/lib)
MESSAGE(STATUS "CBLAS location is: " ${blas})

target_link_libraries(${PROJECT_NAME} ${blas})