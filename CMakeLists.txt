cmake_minimum_required(VERSION 4.0.0)

# Download vcpkg for dependencies
set(VCPKG_VERSION 2025.09.17)
file(DOWNLOAD https://github.com/microsoft/vcpkg/archive/refs/tags/${VCPKG_VERSION}.zip ${CMAKE_BINARY_DIR}/vcpkg.zip)
file(ARCHIVE_EXTRACT INPUT ${CMAKE_BINARY_DIR}/vcpkg.zip)
execute_process(COMMAND cmd /C "${CMAKE_BINARY_DIR}/vcpkg-${VCPKG_VERSION}/bootstrap-vcpkg.bat")
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_BINARY_DIR}/vcpkg-${VCPKG_VERSION}/scripts/buildsystems/vcpkg.cmake)

project(SFML-DOOM)

file(GLOB_RECURSE SRC CONFIGURE_DEPENDS linuxdoom-1.10/*.c linuxdoom-1.10/*.cxx linuxdoom-1.10/*.h)

add_executable(${PROJECT_NAME} ${SRC})

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)
find_package(SFML COMPONENTS Graphics Audio CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE SFML::Graphics SFML::Audio)
set_target_properties(${PROJECT_NAME} PROPERTIES 
    DEBUGGER_WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    XCODE_GENERATE_SCHEME ON
    MACOSX_BUNDLE ON
    COMPILE_WARNING_AS_ERROR ON)
set_property(DIRECTORY . PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
