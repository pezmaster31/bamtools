# ==========================
# BamTools CMakeLists.txt
# (c) 2010 Derek Barnett
#
# src/api/
# ==========================

# list include paths
include_directories( ${BamTools_SOURCE_DIR}/src )

# add compiler definitions 
add_definitions( -DBAMTOOLS_API_LIBRARY ) # (for proper exporting of library symbols)

# create main BamTools API shared library
add_library( BamTools SHARED
             BamAlignment.cpp
             BamIndex.cpp
             BamMultiReader.cpp
             BamReader.cpp
             BamWriter.cpp
             BGZF.cpp
             internal/BamMultiReader_p.cpp
             internal/BamReader_p.cpp
             internal/BamStandardIndex_p.cpp
             internal/BamToolsIndex_p.cpp
             internal/BamWriter_p.cpp
           )
# set shared lib properties
set_target_properties( BamTools PROPERTIES SOVERSION "0.9.0" )
set_target_properties( BamTools PROPERTIES OUTPUT_NAME "bamtools" )

# create main BamTools API static library
add_library( BamTools-static STATIC
             BamAlignment.cpp
             BamIndex.cpp
             BamMultiReader.cpp
             BamReader.cpp
             BamWriter.cpp
             BGZF.cpp
             internal/BamMultiReader_p.cpp
             internal/BamReader_p.cpp
             internal/BamStandardIndex_p.cpp
             internal/BamToolsIndex_p.cpp
             internal/BamWriter_p.cpp
           )
# set static lib properties
set_target_properties( BamTools-static PROPERTIES OUTPUT_NAME "bamtools" )
set_target_properties( BamTools-static PROPERTIES PREFIX "lib" )

# link libraries with zlib automatically
target_link_libraries( BamTools z )
target_link_libraries( BamTools-static z )

# set library install destinations
install( TARGETS BamTools LIBRARY DESTINATION "lib/bamtools")
install( TARGETS BamTools-static ARCHIVE DESTINATION "lib/bamtools")

# export API headers
include(../ExportHeader.cmake)
set(ApiIncludeDir "api")
ExportHeader(APIHeaders api_global.h     ${ApiIncludeDir})
ExportHeader(APIHeaders BamAlignment.h   ${ApiIncludeDir})
ExportHeader(APIHeaders BamAux.h         ${ApiIncludeDir})
ExportHeader(APIHeaders BamIndex.h       ${ApiIncludeDir})
ExportHeader(APIHeaders BamMultiReader.h ${ApiIncludeDir})
ExportHeader(APIHeaders BamReader.h      ${ApiIncludeDir})
ExportHeader(APIHeaders BamWriter.h      ${ApiIncludeDir})
ExportHeader(APIHeaders BGZF.h           ${ApiIncludeDir})
