cmake_minimum_required(VERSION 3.17.3)
project(http_proxy_v2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")

add_compile_options(-ggdb3 -O0 -Wall -Wextra -Werror -Wshadow -Wold-style-cast -Wcast-align -Wunused -Woverloaded-virtual -Wpedantic)

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(http_proxy_v2 ${SOURCES}
        src/Config.h
        src/ClientInfo.h)
