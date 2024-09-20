cmake_minimum_required(VERSION 3.29)

project(sfml-doom)

file(GLOB SOURCE CONFIGURE_DEPENDS linuxdoom-1.10/*.c linuxdoom-1.10/*.cpp linuxdoom-1.10/*.h)

add_executable(${CMAKE_PROJECT_NAME} MACOSX_BUNDLE ${SOURCE})
target_compile_features(${CMAKE_PROJECT_NAME} PRIVATE cxx_std_23)

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE linuxdoom-1.10)

# c++ modules
file(GLOB MODULE_SOURCE CONFIGURE_DEPENDS src/*.ixx)
target_sources(${CMAKE_PROJECT_NAME} PRIVATE FILE_SET CXX_MODULES FILES ${MODULE_SOURCE})

set(FETCHCONTENT_UPDATES_DISCONNECTED ON)
include(FetchContent)

set(BUILD_SHARED_LIBS OFF)

FetchContent_Declare(
  sfml
  GIT_REPOSITORY    https://github.com/sfml/sfml
  GIT_TAG           master
)
FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog
        GIT_TAG v1.14.1
)
FetchContent_MakeAvailable(sfml spdlog)

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE sfml-graphics spdlog::spdlog)

if (UNIX)
    target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE -Werror)
endif()

set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES XCODE_GENERATE_SCHEME YES XCODE_SCHEME_WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

# On apple devices symlink the wads folder into the bundle
if(APPLE)
    add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_SOURCE_DIR}/wads $<TARGET_BUNDLE_DIR:${CMAKE_PROJECT_NAME}>/Contents/MacOS/wads)
endif()