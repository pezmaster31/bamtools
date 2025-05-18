cmake_minimum_required(VERSION 3.11)
# need 3.11 for INCLUDE_GUARD_NAME in GENERATE_EXPORT_HEADER

project(
    BamTools
    LANGUAGES CXX
    VERSION 2.5.3)

# Set Release type for builds where CMAKE_BUILD_TYPE is unset
# This is usually a good default as this implictly enables
#
#   CXXFLAGS = -O3 -DNDEBUG
#
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Adhere to GNU filesystem layout conventions
include(GNUInstallDirs)

# By default build bamtools as a static library
# Most users will prefer static libraries, distributions
# can always switch the standard CMake variable over to ON.
option(BUILD_SHARED_LIBS "Build all libraries as shared" OFF)

# If planning to run in Node.js environment, run:
# cmake -DEnableNodeJS=true
option(EnableNodeJS "Enable running in a Node.js environment" OFF)

# find system JsonCpp
find_package(PkgConfig)
pkg_search_module(JSONCPP jsoncpp>=1)

set(BAMTOOLS_PRIVATE_DEPS zlib)

if(JSONCPP_FOUND)
    message("Found system JsonCpp, not using bundled version")
    set(BAMTOOLS_PRIVATE_DEPS "${BAMTOOLS_PRIVATE_DEPS} jsoncpp")
else()
    message("Did NOT find system JsonCpp, instead using bundled version")
    set(JSONCPP_LDFLAGS jsoncpp)
    set(JSONCPP_INCLUDE_DIRS ${BamTools_SOURCE_DIR}/src/third_party/jsoncpp)
endif()

add_subdirectory(src)

# tests
include(CTest)
add_subdirectory(tests)
