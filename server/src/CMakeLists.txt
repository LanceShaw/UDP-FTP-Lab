cmake_minimum_required(VERSION 3.5)
project(server)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES common.c common.h server.c CommandHandle.c CommandHandle.h server.h main.c)
add_executable(server ${SOURCE_FILES})

find_package(Threads REQUIRED)
target_link_libraries(server Threads::Threads)