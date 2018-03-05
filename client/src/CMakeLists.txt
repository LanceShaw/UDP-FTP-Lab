cmake_minimum_required(VERSION 3.5)
project(client)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES client.c main.c client.h CommandHandle.c CommandHandle.h common.c common.h)
add_executable(client ${SOURCE_FILES})