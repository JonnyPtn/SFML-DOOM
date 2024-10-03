cmake_minimum_required(VERSION 3.28)

# Project name and main executable
project(sfml-doom)
add_executable(${CMAKE_PROJECT_NAME} MACOSX_BUNDLE)

# We use c++23
target_compile_features(${CMAKE_PROJECT_NAME} PRIVATE cxx_std_23)

# Legacy cpp files
file(GLOB SOURCE CONFIGURE_DEPENDS linuxdoom-1.10/*.c linuxdoom-1.10/*.cpp linuxdoom-1.10/*.h)
target_sources(${CMAKE_PROJECT_NAME} PRIVATE ${SOURCE})

# c++ modules source
file(GLOB MODULE_SOURCE CONFIGURE_DEPENDS src/*.ixx)
target_sources(${CMAKE_PROJECT_NAME} PRIVATE FILE_SET CXX_MODULES FILES ${MODULE_SOURCE})

# Include directories
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE linuxdoom-1.10)

# We want to treat compiler warnings as errors
set_property(TARGET ${CMAKE_PROJECT_NAME} PROPERTY COMPILE_WARNING_AS_ERROR YES)

# Use fetch content to fetch dependencies if they are not found
include(FetchContent)

# SFML we take from master - can pin to an SFML 3 release when there is one
# We use this for window, audio and graphics
set(SFML_STATIC_LIBRARIES ON)
set(SFML_FIND_COMPONENTS Graphics Audio)
FetchContent_Declare(
        sfml
        GIT_REPOSITORY    https://github.com/sfml/sfml
        GIT_TAG           master
        FIND_PACKAGE_ARGS
)

# spdlog we use for logging
FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog
        GIT_TAG v1.14.1
        FIND_PACKAGE_ARGS
)

# Make them available
FetchContent_MakeAvailable(sfml spdlog)

# Link them
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE SFML::Graphics SFML::Audio spdlog::spdlog)

# Sym link the wads folder into the build output
if(APPLE)
    set(WADS_DEST $<TARGET_BUNDLE_DIR:${CMAKE_PROJECT_NAME}>/Contents/MacOS/wads)
else()
    set(WADS_DEST $<TARGET_FILE_DIR:${CMAKE_PROJECT_NAME}>/wads)
endif()

add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_SOURCE_DIR}/wads ${WADS_DEST})

# VS startup project should be sfml-doom
set_property(DIRECTORY . PROPERTY VS_STARTUP_PROJECT ${CMAKE_PROJECT_NAME})

# VS working directory should be the output directory
set_property(TARGET ${CMAKE_PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:${CMAKE_PROJECT_NAME}>)