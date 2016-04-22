cmake_minimum_required(VERSION 3.0)

project(doom)

find_package(SFML 2.3 COMPONENTS graphics audio window system)

file(GLOB SOURCE "*.h" "*.hpp" "*.cpp")

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${SFML_INCLUDE_DIR})
add_executable(doom ${SOURCE})
target_link_libraries(doom ${SFML_LIBRARIES} ${SFML_DEPENDENCIES})