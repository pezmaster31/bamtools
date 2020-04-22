# ==========================
# BamTools CMakeLists.txt
# (c) 2010 Derek Barnett
#
# top-level
# ==========================

# CMake requirements
cmake_minimum_required( VERSION 3.0 )

# set project name and version
project( BamTools LANGUAGES CXX VERSION 2.5.1 )

# Set Release type for builds where CMAKE_BUILD_TYPE is unset
# This is usually a good default as this implictly enables
#
#   CXXFLAGS = -O3 -DNDEBUG
#
if( NOT CMAKE_BUILD_TYPE )
    set( CMAKE_BUILD_TYPE "Release" )
endif()

# Need C++11 due to JsonCpp nowadays
set( CMAKE_CXX_STANDARD 11 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )
set( CMAKE_CXX_EXTENSIONS OFF )

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
if( NOT WIN32 )
    add_definitions( -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE )
    add_compile_options( -Wall )
endif()

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

set( BAMTOOLS_PRIVATE_DEPS "zlib" )

if( JSONCPP_FOUND )
    message( "Found system JsonCpp, not using bundled version" )
    set( BAMTOOLS_PRIVATE_DEPS "${BAMTOOLS_PRIVATE_DEPS} jsoncpp" )
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

# tests
include( CTest )
add_subdirectory( tests )
