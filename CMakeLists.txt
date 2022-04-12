cmake_minimum_required(VERSION 3.22)

project(sfml-doom)

file(GLOB SOURCE linuxdoom-1.10/*.c linuxdoom-1.10/*.h)

add_executable(${CMAKE_PROJECT_NAME} ${SOURCE})

if (UNIX)
    target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE -w)
endif()