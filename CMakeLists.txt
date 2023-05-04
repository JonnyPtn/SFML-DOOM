cmake_minimum_required(VERSION 3.21)

project(sfml-doom)

file(GLOB SOURCE CONFIGURE_DEPENDS linuxdoom-1.10/*.c linuxdoom-1.10/*.cpp linuxdoom-1.10/*.h)

add_executable(${CMAKE_PROJECT_NAME} MACOSX_BUNDLE ${SOURCE})
target_compile_features(${CMAKE_PROJECT_NAME} PRIVATE cxx_std_20)

set(FETCHCONTENT_UPDATES_DISCONNECTED ON)
include(FetchContent)

FetchContent_Declare(
  sfml
  GIT_REPOSITORY    https://github.com/sfml/sfml
  GIT_TAG           master
)
FetchContent_MakeAvailable(sfml)

FetchContent_Declare(
  imgui
  GIT_REPOSITORY    https://github.com/ocornut/imgui
  GIT_TAG           v1.87
)
FetchContent_MakeAvailable(imgui)

set(IMGUI_SFML_FIND_SFML OFF)
set(IMGUI_DIR ${imgui_SOURCE_DIR})
FetchContent_Declare(
  imgui-sfml
  GIT_REPOSITORY    https://github.com/eliasdaler/imgui-sfml
  GIT_TAG           master
)
FetchContent_MakeAvailable(imgui-sfml)

file(DOWNLOAD https://distro.ibiblio.org/slitaz/sources/packages/d/doom1.wad doom1.wad 
     SHOW_PROGRESS EXPECTED_MD5 f0cefca49926d00903cf57551d901abe)

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE sfml-graphics ImGui-SFML::ImGui-SFML)

if (UNIX)
    target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE -Werror)
elseif(MSVC)
    target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE /w)
endif()

set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES XCODE_GENERATE_SCHEME YES XCODE_SCHEME_WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})