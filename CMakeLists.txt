cmake_minimum_required(VERSION 3.22)

project(sfml-doom)

file(GLOB SOURCE CONFIGURE_DEPENDS linuxdoom-1.10/*.c linuxdoom-1.10/*.cpp linuxdoom-1.10/*.h)

add_executable(${CMAKE_PROJECT_NAME} ${SOURCE})
target_compile_features(${CMAKE_PROJECT_NAME} PRIVATE cxx_std_17)

find_package(SFML REQUIRED COMPONENTS Graphics)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE SFML::Graphics)

if (UNIX)
    target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE -w)
endif()