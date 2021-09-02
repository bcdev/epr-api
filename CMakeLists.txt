# Copyright (c) 2013-2014 Antonio Valentino <antonio.valentino@tiscali.it>


cmake_minimum_required(VERSION 2.8)
project(epr-api)


# set version number
set(EPR_API_VERSION_MAJOR 2)
set(EPR_API_VERSION_MINOR 3)
set(EPR_API_VERSION_PATCH 0)
set(EPR_API_VERSION_STRING
    "${EPR_API_VERSION_MAJOR}.${EPR_API_VERSION_MINOR}.${EPR_API_VERSION_PATCH}")


# NOTE: only change this when an ABI change happens
set(EPR_API_SOVERSION 3)


# options
option(BUILD_STATIC_LIB "build the static version of the library" ON)
option(BUILD_TESTS "build test programs" OFF)
option(BUILD_DOCS "build Doxygen documentation" OFF)
option(DISABLE_SYMBOL_CONTROL
    "Do not try to control symbols that are exported by the dynamic library.
This option can be used to fix some build issue with non standard linkers."
    OFF)


# testing
enable_testing()


# sub-directories
add_subdirectory(src)

if(BUILD_TESTS)
    add_subdirectory(bccunit)
endif(BUILD_TESTS)

if(BUILD_DOCS)
    add_subdirectory(docs)
endif(BUILD_DOCS)


# uninstall target
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)


# packaging
include(InstallRequiredSystemLibraries)

set(CPACK_GENERATOR TGZ ZIP STGZ)
set(CPACK_SOURCE_GENERATOR TGZ ZIP)
set(CPACK_PACKAGE_VERSION_MAJOR ${EPR_API_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${EPR_API_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${EPR_API_VERSION_PATCH})
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.txt")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY
    "Delft object-oriented radar interferometric software")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.txt")
set(CPACK_SOURCE_IGNORE_FILES "/build.*;.*~;\\\\.git.*;\\\\.user$;\\\\.DS_Store")
set(CPACK_STRIP_FILES TRUE)
set(CPACK_SOURCE_STRIP_FILES TRUE)

include(CPack)

