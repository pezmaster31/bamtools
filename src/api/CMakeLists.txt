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

# fetch all internal source files
add_subdirectory( internal )

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

# link libraries automatically with zlib (and Winsock2, if applicable)
find_package( ZLIB REQUIRED )

if( WIN32 )
    set( WIN32_LIBRARIES wsock32 ws2_32 )
endif()

# create main BamTools API library
add_library( BamTools ${BamToolsAPISources} )
# The SONAME is bumped on every version increment
# as Bamtools does not yet guarantee a stable ABI
set_target_properties( BamTools PROPERTIES
                       SOVERSION ${BamTools_VERSION}
                       OUTPUT_NAME bamtools )
target_include_directories( BamTools PRIVATE ${ZLIB_INCLUDE_DIRS} )
target_link_libraries( BamTools PRIVATE ${ZLIB_LIBRARIES} ${WIN32_LIBRARIES} )
install( TARGETS BamTools
         ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
         LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
         RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} )

install( FILES api_global.h              DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/bamtools/api COMPONENT Development )
install( FILES BamAlgorithms.h           DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/bamtools/api COMPONENT Development )
install( FILES BamAlignment.h            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/bamtools/api COMPONENT Development )
install( FILES BamAux.h                  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/bamtools/api COMPONENT Development )
install( FILES BamConstants.h            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/bamtools/api COMPONENT Development )
install( FILES BamIndex.h                DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/bamtools/api COMPONENT Development )
install( FILES BamMultiReader.h          DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/bamtools/api COMPONENT Development )
install( FILES BamReader.h               DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/bamtools/api COMPONENT Development )
install( FILES BamWriter.h               DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/bamtools/api COMPONENT Development )
install( FILES IBamIODevice.h            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/bamtools/api COMPONENT Development )
install( FILES SamConstants.h            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/bamtools/api COMPONENT Development )
install( FILES SamHeader.h               DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/bamtools/api COMPONENT Development )
install( FILES SamProgram.h              DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/bamtools/api COMPONENT Development )
install( FILES SamProgramChain.h         DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/bamtools/api COMPONENT Development )
install( FILES SamReadGroup.h            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/bamtools/api COMPONENT Development )
install( FILES SamReadGroupDictionary.h  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/bamtools/api COMPONENT Development )
install( FILES SamSequence.h             DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/bamtools/api COMPONENT Development )
install( FILES SamSequenceDictionary.h   DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/bamtools/api COMPONENT Development )

install( FILES algorithms/Sort.h         DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/bamtools/api/algorithms COMPONENT Development )
