# ==========================
# BamTools CMakeLists.txt
# (c) 2010 Derek Barnett
#
# src/third-party/
# ==========================

# list third-party subdirectories to build in
if( NOT JSONCPP_FOUND )
    add_subdirectory( jsoncpp )
endif()
