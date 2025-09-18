cmake_minimum_required(VERSION 4.0.0)

project(SFML-DOOM)

file(GLOB_RECURSE SRC CONFIGURE_DEPENDS linuxdoom-1.10/*.c linuxdoom-1.10/*.cxx linuxdoom-1.10/*.h)

add_executable(${PROJECT_NAME} ${SRC})

include(FetchContent)

FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.1
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

target_compile_options(${PROJECT_NAME} PRIVATE -w)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)
target_link_libraries(${PROJECT_NAME} PRIVATE SFML::Graphics)
target_compile_definitions(${PROJECT_NAME} PRIVATE _CRT_SECURE_NO_WARNINGS=1)
set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
set_property(DIRECTORY . PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})