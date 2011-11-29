# ==========================
# BamTools CMakeLists.txt
# (c) 2010 Derek Barnett
#
# src/test
# ==========================

# specify source include directories
include_directories( ${BamTools_SOURCE_DIR}/src/api
                     ${BamTools_SOURCE_DIR}/src/test
                     ${BamTools_SOURCE_DIR}/src/third_party
                     ${BamTools_SOURCE_DIR}/src/third_party/gtest-1.6.0/include
                     ${BamTools_SOURCE_DIR}/src/utils
                   )

# create our test app 'test_bamtools'
add_executable( BamTools-test EXCLUDE_FROM_ALL
                TestMain.cpp
              )
set_target_properties( BamTools-test PROPERTIES
                       OUTPUT_NAME "test_bamtools" )
target_link_libraries( BamTools-test BamTools BamTools-utils jsoncpp gtest )

# add test - running 'make check' will ensure build & run
add_test( BamToolsTest BamTools-test )
add_custom_target( check COMMAND ${EXECUTABLE_OUTPUT_PATH}/test_bamtools
                         DEPENDS BamTools-test gtest )

