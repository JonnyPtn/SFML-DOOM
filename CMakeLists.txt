cmake_minimum_required(VERSION 3.29)

project(sfml-doom)

file(GLOB SOURCE CONFIGURE_DEPENDS linuxdoom-1.10/*.c linuxdoom-1.10/*.cpp linuxdoom-1.10/*.h)

add_executable(${CMAKE_PROJECT_NAME} MACOSX_BUNDLE ${SOURCE})
target_compile_features(${CMAKE_PROJECT_NAME} PRIVATE cxx_std_20)

set(FETCHCONTENT_UPDATES_DISCONNECTED ON)
include(FetchContent)

set(BUILD_SHARED_LIBS OFF)

FetchContent_Declare(
  sfml
  GIT_REPOSITORY    https://github.com/sfml/sfml
  GIT_TAG           master
)
FetchContent_MakeAvailable(sfml)

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE sfml-graphics)

if (UNIX)
    target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE -Werror)
endif()

set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES XCODE_GENERATE_SCHEME YES XCODE_SCHEME_WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

