cmake_minimum_required(VERSION 3.21)
project(notebook C)

set(CMAKE_C_STANDARD 99)

add_executable(notebook main.c students_repo.c utils.c smart_buffer.c students_service.c student.c)
target_link_libraries(${PROJECT_NAME} sqlite3)