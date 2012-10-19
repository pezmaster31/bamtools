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
add_definitions( -fPIC ) # (attempt to force PIC compiling on CentOS, not being set on shared libs by CMake)

# fetch all internal source files
add_subdirectory ( internal )

# make list of all API source files
set( BamToolsAPISources
        BamAlignment.cpp
        BamMultiReader.cpp
        BamReader.cpp
        BamWriter.cpp
        SamHeader.cpp
        SamProgram.cpp
        SamProgramChain.cpp
        SamReadGroup.cpp
        SamReadGroupDictionary.cpp
        SamSequence.cpp
        SamSequenceDictionary.cpp
        ${InternalSources}
)

# create main BamTools API shared library
add_library( BamTools SHARED ${BamToolsAPISources} )
set_target_properties( BamTools PROPERTIES
                       SOVERSION "2.1.0"
                       OUTPUT_NAME "bamtools" )

# create main BamTools API static library
add_library( BamTools-static STATIC ${BamToolsAPISources} )
set_target_properties( BamTools-static PROPERTIES 
                       OUTPUT_NAME "bamtools" 
                       PREFIX "lib" )

# link libraries automatically with zlib (and Winsock2, if applicable)
if( _WIN32 )
    set( APILibs z ws2_32 )
else( _WIN32 )
    set( APILibs z )
endif( _WIN32 )

target_link_libraries( BamTools ${APILibs} )
target_link_libraries( BamTools-static ${APILibs} )

# set library install destinations
install( TARGETS BamTools
         LIBRARY DESTINATION "lib/bamtools"
         RUNTIME DESTINATION "bin")

install( TARGETS BamTools-static
         ARCHIVE DESTINATION "lib/bamtools")

install( FILES api_global.h              DESTINATION "include/bamtools/api" COMPONENT Development )
install( FILES BamAlgorithms.h           DESTINATION "include/bamtools/api" COMPONENT Development )
install( FILES BamAlignment.h            DESTINATION "include/bamtools/api" COMPONENT Development )
install( FILES BamAux.h                  DESTINATION "include/bamtools/api" COMPONENT Development )
install( FILES BamConstants.h            DESTINATION "include/bamtools/api" COMPONENT Development )
install( FILES BamIndex.h                DESTINATION "include/bamtools/api" COMPONENT Development )
install( FILES BamMultiReader.h          DESTINATION "include/bamtools/api" COMPONENT Development )
install( FILES BamReader.h               DESTINATION "include/bamtools/api" COMPONENT Development )
install( FILES BamWriter.h               DESTINATION "include/bamtools/api" COMPONENT Development )
install( FILES IBamIODevice.h            DESTINATION "include/bamtools/api" COMPONENT Development )
install( FILES SamConstants.h            DESTINATION "include/bamtools/api" COMPONENT Development )
install( FILES SamHeader.h               DESTINATION "include/bamtools/api" COMPONENT Development )
install( FILES SamProgram.h              DESTINATION "include/bamtools/api" COMPONENT Development )
install( FILES SamProgramChain.h         DESTINATION "include/bamtools/api" COMPONENT Development )
install( FILES SamReadGroup.h            DESTINATION "include/bamtools/api" COMPONENT Development )
install( FILES SamReadGroupDictionary.h  DESTINATION "include/bamtools/api" COMPONENT Development )
install( FILES SamSequence.h             DESTINATION "include/bamtools/api" COMPONENT Development )
install( FILES SamSequenceDictionary.h   DESTINATION "include/bamtools/api" COMPONENT Development )
install( FILES algorithms/Sort.h         DESTINATION "include/bamtools/api/algorithms" COMPONENT Development )
