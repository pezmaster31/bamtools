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

# list of all BamTools API source (.cpp) files
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
        internal/bam/BamHeader_p.cpp
        internal/bam/BamMultiReader_p.cpp
        internal/bam/BamRandomAccessController_p.cpp
        internal/bam/BamReader_p.cpp
        internal/bam/BamWriter_p.cpp
        internal/index/BamIndexFactory_p.cpp
        internal/index/BamStandardIndex_p.cpp
        internal/index/BamToolsIndex_p.cpp
        internal/io/BamDeviceFactory_p.cpp
        internal/io/BamFile_p.cpp
        internal/io/BamFtp_p.cpp
        internal/io/BamHttp_p.cpp
        internal/io/BamPipe_p.cpp
        internal/io/BgzfStream_p.cpp
        internal/io/ILocalIODevice_p.cpp
        internal/io/IRemoteIODevice_p.cpp
        internal/sam/SamFormatParser_p.cpp
        internal/sam/SamFormatPrinter_p.cpp
        internal/sam/SamHeaderValidator_p.cpp
        internal/utils/BamException_p.cpp
)

# create main BamTools API shared library
add_library( BamTools SHARED ${BamToolsAPISources} )
set_target_properties( BamTools PROPERTIES SOVERSION "2.0.5" )
set_target_properties( BamTools PROPERTIES OUTPUT_NAME "bamtools" )

# create main BamTools API static library
add_library( BamTools-static STATIC ${BamToolsAPISources} )
set_target_properties( BamTools-static PROPERTIES OUTPUT_NAME "bamtools" )
set_target_properties( BamTools-static PROPERTIES PREFIX "lib" )

# link libraries with zlib automatically
target_link_libraries( BamTools z )
target_link_libraries( BamTools-static z )

# set library install destinations
install( TARGETS BamTools LIBRARY DESTINATION "lib/bamtools" RUNTIME DESTINATION "bin")
install( TARGETS BamTools-static ARCHIVE DESTINATION "lib/bamtools")

# export API headers
include(../ExportHeader.cmake)
set(ApiIncludeDir "api")
ExportHeader(APIHeaders api_global.h             ${ApiIncludeDir})
ExportHeader(APIHeaders BamAlgorithms.h           ${ApiIncludeDir})
ExportHeader(APIHeaders BamAlignment.h           ${ApiIncludeDir})
ExportHeader(APIHeaders BamAux.h                 ${ApiIncludeDir})
ExportHeader(APIHeaders BamConstants.h           ${ApiIncludeDir})
ExportHeader(APIHeaders BamIndex.h               ${ApiIncludeDir})
ExportHeader(APIHeaders BamMultiReader.h         ${ApiIncludeDir})
ExportHeader(APIHeaders BamReader.h              ${ApiIncludeDir})
ExportHeader(APIHeaders BamWriter.h              ${ApiIncludeDir})
ExportHeader(APIHeaders IBamIODevice.h           ${ApiIncludeDir})
ExportHeader(APIHeaders SamConstants.h           ${ApiIncludeDir})
ExportHeader(APIHeaders SamHeader.h              ${ApiIncludeDir})
ExportHeader(APIHeaders SamProgram.h             ${ApiIncludeDir})
ExportHeader(APIHeaders SamProgramChain.h        ${ApiIncludeDir})
ExportHeader(APIHeaders SamReadGroup.h           ${ApiIncludeDir})
ExportHeader(APIHeaders SamReadGroupDictionary.h ${ApiIncludeDir})
ExportHeader(APIHeaders SamSequence.h            ${ApiIncludeDir})
ExportHeader(APIHeaders SamSequenceDictionary.h  ${ApiIncludeDir})

set(AlgorithmsIncludeDir "api/algorithms")
ExportHeader(AlgorithmsHeaders algorithms/Sort.h ${AlgorithmsIncludeDir})
