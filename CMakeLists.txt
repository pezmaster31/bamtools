# ==========================
# BamTools CMakeLists.txt
# (c) 2010 Derek Barnett
#
# top-level
# ==========================

# CMake requirements
cmake_minimum_required( VERSION 3.0 )

# allow setting project version in project()
# https://cmake.org/cmake/help/v3.0/policy/CMP0048.html#policy:CMP0048
cmake_policy( SET CMP0048 NEW )

# set project name and version
project( BamTools LANGUAGES CXX VERSION 2.4.1 )

# on macOS, MACOSX_RPATH is enabled by default on more recent versions
# of CMake. Disable this behaviour, and let user enable it if need be.
cmake_policy( SET CMP0042 OLD )

# Set Release type for builds where CMAKE_BUILD_TYPE is unset
# This is usually a good default as this implictly enables
#
#   CXXFLAGS = -O3 -DNDEBUG
#
if( NOT CMAKE_BUILD_TYPE )
    set( CMAKE_BUILD_TYPE "Release" )
endif()

# Adhere to GNU filesystem layout conventions
include( GNUInstallDirs )

# Force the build directory to be different from source directory
macro( ENSURE_OUT_OF_SOURCE_BUILD MSG )
    string( COMPARE EQUAL "${CMAKE_SOURCE_DIR}" "${CMAKE_BINARY_DIR}" insource )
    get_filename_component( PARENTDIR ${CMAKE_SOURCE_DIR} PATH )
    string( COMPARE EQUAL "${CMAKE_SOURCE_DIR}" "${PARENTDIR}" insourcesubdir )
    IF( insource OR insourcesubdir )
        message( FATAL_ERROR "${MSG}" )
    ENDIF( insource OR insourcesubdir )
endmacro( ENSURE_OUT_OF_SOURCE_BUILD )

ensure_out_of_source_build( "
  ${PROJECT_NAME} requires an out of source build.
  $ mkdir build
  $ cd build
  $ cmake ..
  $ make
(or the Windows equivalent)\n" )

# define compiler flags for all code, copied from Autoconf's AC_SYS_LARGEFILE
add_definitions( -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE )
add_compile_options( -Wall )

# -----------------------------------------------
# handle platform-/environment-specific defines

# By default build bamtools as a static library
# Most users will prefer static libraries, distributions
# can always switch the standard CMake variable over to ON.
set( BUILD_SHARED_LIBS OFF CACHE BOOL "Build all libraries as shared" )

# If planning to run in Node.js environment, run:
# cmake -DEnableNodeJS=true
if( EnableNodeJS )
    add_definitions( -DSYSTEM_NODEJS=1 )
endif()

# If running on SunOS
if( "${CMAKE_SYSTEM_NAME}" MATCHES "SunOS" )
    add_definitions( -DSUN_OS )
endif()

# find system JsonCpp
find_package( PkgConfig )
pkg_search_module( JSONCPP jsoncpp>=1 )

if( JSONCPP_FOUND )
    message( "Found system JsonCpp, not using bundled version" )
else()
    message( "Did NOT find system JsonCpp, instead using bundled version" )
    set( JSONCPP_LDFLAGS jsoncpp )
    set( JSONCPP_INCLUDE_DIRS ${BamTools_SOURCE_DIR}/src/third_party/jsoncpp )
endif()


# -------------------------------------------

# add our includes root path
include_directories( src )

# list subdirectories to build in
add_subdirectory( src )
