cmake_minimum_required(VERSION 2.8)

project(SFML-DOOM)

# Version number
SET(CPACK_PACKAGE_VERSION_MAJOR 0)
SET(CPACK_PACKAGE_VERSION_MINOR 2)
SET(CPACK_PACKAGE_VERSION_PATCH 0)

# Only works with SFML version 2.4 and above
SET(SFML_MIN_VERSION 2.4)
if (IOS)
  # I'm not sure right now why audio isn't available
  set(SFML_STATIC_LIBRARIES TRUE) 
  add_definitions(-DSFML_DOOM_IOS)
  set(CMAKE_FIND_FRAMEWORK LAST)
  find_host_package(SFML REQUIRED COMPONENTS graphics audio window system network main)
else()
  find_package(SFML REQUIRED COMPONENTS graphics audio window system network)
endif()

file(GLOB INCLUDE "include/*.h" "include/*.hpp" "include/timidity/*.h")
file(GLOB SOURCE "src/*.cpp" "src/timidity/*.cpp")

if (APPLE)
    set(SOURCE ${SOURCE} src/ResourcePath.mm)
endif()

set (CMAKE_CXX_STANDARD 11)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include ${SFML_INCLUDE_DIR})
if(APPLE)

    # Set some details for the app bundle. Without these my xcode crashes when debugging ios :S
    set(MACOSX_BUNDLE_GUI_IDENTIFIER SFML-DOOM)
    set(MACOSX_BUNDLE_BUNDLE_NAME SFML-DOOM)

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

target_link_libraries(SFML-DOOM PRIVATE sfml-graphics sfml-audio sfml-network)

install(TARGETS SFML-DOOM BUNDLE DESTINATION .
			  RUNTIME DESTINATION .)

# If not on apple, we need to install the resources
if (NOT APPLE)
    install(DIRECTORY ${CMAKE_SOURCE_DIR}/eawpats
	    DESTINATION .)
    install(FILES ${CMAKE_SOURCE_DIR}/doom1.wad
            DESTINATION .)
endif()

#Platform specific packaging
if(APPLE)
	set(CPACK_PACKAGE_FILE_NAME)
    install(CODE " include(BundleUtilities)
		 fixup_bundle(${CMAKE_INSTALL_PREFIX}/SFML-DOOM.app \"\" \"/Library/Frameworks\")
		 verify_app(${CMAKE_INSTALL_PREFIX}/SFML-DOOM.app)")
    SET(CPACK_GENERATOR DragNDrop)
elseif(UNIX)
	set(CPACK_GENERATOR DEB)
	set(CPACK_PACKAGE_CONTACT Jonny@JonnyPaton.com)
elseif(WIN32)
	set(CPACK_GENERATOR NSIS)
endif()


include(CPack)
