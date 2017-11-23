cmake_minimum_required(VERSION 2.8)

project(SFML-DOOM)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake_modules")

find_package(SFML 2.4 COMPONENTS graphics audio window system network)

file(GLOB INCLUDE "include/*.h" "include/*.hpp" "include/timidity/*.h") 
file(GLOB SOURCE "src/*.cpp" "src/timidity/*.cpp" "src/*.mm")

set (CMAKE_CXX_STANDARD 11)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include ${SFML_INCLUDE_DIR})
if(APPLE)
    set_source_files_properties( ${CMAKE_SOURCE_DIR}/apple/sfml-doom.icns PROPERTIES MACOSX_PACKAGE_LOCATION Resources )
    set_source_files_properties( ${CMAKE_SOURCE_DIR}/eawpats PROPERTIES MACOSX_PACKAGE_LOCATION Resources )
    set_source_files_properties( ${CMAKE_SOURCE_DIR}/doom1.wad PROPERTIES MACOSX_PACKAGE_LOCATION Resources )
    add_executable(SFML-DOOM MACOSX_BUNDLE ${INCLUDE} ${SOURCE} ${CMAKE_SOURCE_DIR}/apple/sfml-doom.icns
    ${CMAKE_SOURCE_DIR}/doom1.wad
    ${CMAKE_SOURCE_DIR}/eawpats)
    set_target_properties( SFML-DOOM PROPERTIES MACOSX_BUNDLE_ICON_FILE sfml-doom.icns )
else()
    add_executable(SFML-DOOM ${INCLUDE} ${SOURCE})
endif()
target_link_libraries(SFML-DOOM ${SFML_LIBRARIES} ${SFML_DEPENDENCIES})

install(TARGETS SFML-DOOM  DESTINATION .)
install(FILES doom1.wad DESTINATION .)
install(DIRECTORY eawpats DESTINATION .)

include(cpack)
