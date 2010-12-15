# ==========================
# BamTools CMakeLists.txt
# (c) 2010 Derek Barnett
#
# src/
# ==========================

add_subdirectory (api)
add_subdirectory (utils)
add_subdirectory (third_party)
add_subdirectory (toolkit)

# export shared headers
include(ExportHeader.cmake)
set(SharedIncludeDir "shared")
ExportHeader(SharedHeaders shared/bamtools_global.h ${SharedIncludeDir})


