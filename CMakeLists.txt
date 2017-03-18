cmake_minimum_required(VERSION 2.8)

project(SFML-DOOM)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake_modules")

find_package(SFML 2.4 COMPONENTS graphics audio window system network)

file(GLOB SOURCE "src/*.h" "src/*.hpp" "src/*.cpp" "src/timidity/*.cpp" "src/timidity/*.h") 

set (CMAKE_CXX_STANDARD 11)

#for backwards compatibility
if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "-std=c++11 -fno-strict-aliasing")
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${SFML_INCLUDE_DIR})
add_executable(SFML-DOOM ${SOURCE})
target_link_libraries(SFML-DOOM ${SFML_LIBRARIES} ${SFML_DEPENDENCIES})
