cmake_minimum_required(VERSION 2.8)

project(SFML-DOOM)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake_modules")

find_package(SFML 2.4 COMPONENTS graphics audio window system network)

file(GLOB INCLUDE "include/*.h" "include/*.hpp" "include/timidity/*.h") 
file(GLOB SOURCE "src/*.cpp" "src/timidity/*.cpp")

set (CMAKE_CXX_STANDARD 11)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include ${SFML_INCLUDE_DIR})
add_executable(SFML-DOOM ${INCLUDE} ${SOURCE})
target_link_libraries(SFML-DOOM ${SFML_LIBRARIES} ${SFML_DEPENDENCIES})
