# Use vcpkg if available, if not fetchcontent
if(DEFINED ENV{VCPKG_ROOT})
    cmake_path(SET CMAKE_TOOLCHAIN_FILE NORMALIZE $ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake)
    set(CMAKE_FIND_REQUIRED_SFML TRUE)

    # Static triplets by default
    if(CMAKE_HOST_WIN32)
        set(VCPKG_TARGET_TRIPLET x64-windows-static)
    elseif(CMAKE_HOST_APPLE)
        set(VCPKG_TARGET_TRIPLET arm64-osx)
    else()
        set(VCPKG_TARGET_TRIPLET x64-linux)
    endif()
else()
    include(FetchContent)
    FetchContent_Declare(SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 3.0.2
        GIT_SHALLOW ON
        EXCLUDE_FROM_ALL
        SYSTEM)
    FetchContent_MakeAvailable(SFML)
endif()