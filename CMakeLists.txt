cmake_minimum_required(VERSION 4.0.0)

project(SFML-DOOM)

file(GLOB_RECURSE SRC CONFIGURE_DEPENDS linuxdoom-1.10/*.c linuxdoom-1.10/*.cxx linuxdoom-1.10/*.h)

add_executable(${PROJECT_NAME} ${SRC})

target_compile_options(${PROJECT_NAME} PRIVATE -w)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)