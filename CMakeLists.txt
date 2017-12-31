cmake_minimum_required(VERSION 2.8)

project(SFML-DOOM)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake/modules")

# Only works with SFML version 2.4 and above
SET(SFML_MIN_VERSION 2.4)
if (IOS)
  # I'm not sure right now why audio isn't available
  set(SFML_STATIC_LIBRARIES TRUE)
  set(SFML_IOS TRUE)
  find_host_package(SFML REQUIRED graphics audio window system network main)
else()
  find_package(SFML REQUIRED graphics audio window system network)
endif()

file(GLOB INCLUDE "include/*.h" "include/*.hpp" "include/timidity/*.h")
file(GLOB SOURCE "src/*.cpp" "src/timidity/*.cpp")

if (APPLE AND NOT IOS)
    set(SOURCE ${SOURCE} src/ResourcePath.mm)
endif()

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
    include(BundleUtilities)
else()
    add_executable(SFML-DOOM ${INCLUDE} ${SOURCE})
endif()
target_link_libraries(SFML-DOOM ${SFML_LIBRARIES} ${SFML_DEPENDENCIES})

install(TARGETS SFML-DOOM BUNDLE DESTINATION ".")

#Platform specific packaging
if(APPLE)
	set(CPACK_PACKAGE_FILE_NAME)
    install(CODE " include(BundleUtilities)
		 fixup_bundle(${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/SFML-DOOM.app \"\" \"\")
		 verify_app(${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/SFML-DOOM.app)")
    SET(CPACK_GENERATOR "DragNDrop")
elseif(UNIX)
	set(CPACK_GENERATOR "DEB")
endif()




include(CPack)
