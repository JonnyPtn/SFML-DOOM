cmake_minimum_required(VERSION 3.31)

include(cmake/dependencies.cmake)

project(SFML-DOOM)

file(GLOB_RECURSE SRC CONFIGURE_DEPENDS linuxdoom-1.10/*.c linuxdoom-1.10/*.cxx linuxdoom-1.10/*.h)

add_executable(${PROJECT_NAME} ${SRC})

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)
find_package(SFML 3 COMPONENTS Graphics Audio CONFIG)
target_link_libraries(${PROJECT_NAME} PRIVATE SFML::Graphics SFML::Audio)
set_target_properties(${PROJECT_NAME} PROPERTIES 
    DEBUGGER_WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    XCODE_GENERATE_SCHEME ON
    MACOSX_BUNDLE ON
    COMPILE_WARNING_AS_ERROR ON)
set_property(DIRECTORY . PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})

target_compile_options(${PROJECT_NAME} PRIVATE -w)

install(TARGETS ${PROJECT_NAME} 
    RUNTIME DESTINATION .
    BUNDLE DESTINATION .)
install(FILES doom1.wad DESTINATION .)

include(CPack)