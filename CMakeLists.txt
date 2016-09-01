cmake_minimum_required(VERSION 3.0)

project(SFML-DOOM)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake_modules")

find_package(SFML 2.4 COMPONENTS graphics audio window system network)

file(GLOB SOURCE "*.h" "*.hpp" "*.cpp")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w -fpermissive -std=c++11 -m32")
include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${SFML_INCLUDE_DIR})
add_executable(SFML-DOOM ${SOURCE})
target_link_libraries(SFML-DOOM ${SFML_LIBRARIES} ${SFML_DEPENDENCIES})