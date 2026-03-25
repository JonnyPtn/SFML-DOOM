cmake_minimum_required(VERSION 3.31)

include(cmake/dependencies.cmake)

project(SFML-DOOM)

file(GLOB_RECURSE SRC CONFIGURE_DEPENDS linuxdoom-1.10/*.c linuxdoom-1.10/*.cxx linuxdoom-1.10/*.h)

set(WAD_FILE ${CMAKE_SOURCE_DIR}/doom1.wad)
set_source_files_properties(${WAD_FILE} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)

add_executable(${PROJECT_NAME} ${SRC} ${WAD_FILE})

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)
find_package(SFML 3 COMPONENTS Graphics Audio CONFIG)
target_link_libraries(${PROJECT_NAME} PRIVATE SFML::Graphics SFML::Audio "$<$<PLATFORM_ID:Darwin>:-framework CoreFoundation>")
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

if (NOT APPLE)
    install(FILES ${WAD_FILE} DESTINATION .)
endif()

include(CPack)