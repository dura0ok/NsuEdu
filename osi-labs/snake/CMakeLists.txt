cmake_minimum_required(VERSION 3.25)
project(untitled C)

set(CMAKE_C_STANDARD 11)


set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address")
#add_executable(server server.c engine.c terminal_library.c)
add_executable(client client.c client_engine.c terminal_library.c)