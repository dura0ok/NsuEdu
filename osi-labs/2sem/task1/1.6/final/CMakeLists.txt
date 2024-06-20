cmake_minimum_required(VERSION 3.16)

project(threads_test)

add_library(mythread SHARED mythread.c)

add_executable(${PROJECT_NAME} main.c)

# Link the mythread library with the executable
target_link_libraries(${PROJECT_NAME} mythread)

# Set compile options
target_compile_options(${PROJECT_NAME} PUBLIC -Og -ggdb "-ffile-prefix-map=${CMAKE_SOURCE_DIR}=.")
target_compile_options(mythread PUBLIC)
