add_subdirectory(third_party)

if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -pedantic)
    # define compiler flags for all code, copied from Autoconf's AC_SYS_LARGEFILE
    add_definitions(-D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE)
endif()

###########
# library #
###########

# needed for reading compressed BAM files
find_package(ZLIB REQUIRED)

# create main BamTools API library
add_library(
    BamTools

    api/BamAlignment.cpp
    api/BamMultiReader.cpp
    api/BamReader.cpp
    api/BamWriter.cpp
    api/SamHeader.cpp
    api/SamProgram.cpp
    api/SamProgramChain.cpp
    api/SamReadGroup.cpp
    api/SamReadGroupDictionary.cpp
    api/SamSequence.cpp
    api/SamSequenceDictionary.cpp
    api/internal/bam/BamHeader_p.cpp
    api/internal/bam/BamMultiReader_p.cpp
    api/internal/bam/BamRandomAccessController_p.cpp
    api/internal/bam/BamReader_p.cpp
    api/internal/bam/BamWriter_p.cpp
    api/internal/index/BamIndexFactory_p.cpp
    api/internal/index/BamStandardIndex_p.cpp
    api/internal/index/BamToolsIndex_p.cpp
    api/internal/io/BamDeviceFactory_p.cpp
    api/internal/io/BamFile_p.cpp
    api/internal/io/BamFtp_p.cpp
    api/internal/io/BamHttp_p.cpp
    api/internal/io/BamPipe_p.cpp
    api/internal/io/BgzfStream_p.cpp
    api/internal/io/ByteArray_p.cpp
    api/internal/io/HostAddress_p.cpp
    api/internal/io/HostInfo_p.cpp
    api/internal/io/HttpHeader_p.cpp
    api/internal/io/ILocalIODevice_p.cpp
    api/internal/io/RollingBuffer_p.cpp
    api/internal/io/TcpSocketEngine_p.cpp
    api/internal/io/TcpSocket_p.cpp
    api/internal/sam/SamFormatParser_p.cpp
    api/internal/sam/SamFormatPrinter_p.cpp
    api/internal/sam/SamHeaderValidator_p.cpp
    api/internal/utils/BamException_p.cpp
)

# The SONAME is bumped on every version increment
# as Bamtools does not yet guarantee a stable ABI
set_target_properties(
    BamTools PROPERTIES
    SOVERSION ${BamTools_VERSION}
    OUTPUT_NAME bamtools
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF)
target_include_directories(
    BamTools PRIVATE
    ${ZLIB_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(
    BamTools PRIVATE
    ${ZLIB_LIBRARIES})

if(WIN32)
    target_link_libraries(
        BamTools PRIVATE
        wsock32 ws2_32)
    target_sources(
        BamTools PRIVATE
        api/internal/io/TcpSocketEngine_win_p.cpp)
else()
    target_sources(
        BamTools PRIVATE
        api/internal/io/TcpSocketEngine_unix_p.cpp)
endif()

if(EnableNodeJS)
    target_compile_definitions(
        BamTools PRIVATE
        SYSTEM_NODEJS=1)
endif()

# convenience library
add_library(
    BamTools-utils
    STATIC
    utils/bamtools_fasta.cpp
    utils/bamtools_options.cpp
    utils/bamtools_pileup_engine.cpp
    utils/bamtools_utilities.cpp
)
set_target_properties(
    BamTools-utils PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF)
target_include_directories(
    BamTools-utils PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(
    BamTools-utils PRIVATE
    BamTools)

####################
# main application #
####################

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/toolkit/bamtools_version.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/bamtools_version.h
    @ONLY)

add_executable(
    bamtools_cmd

    toolkit/bamtools.cpp
    toolkit/bamtools_convert.cpp
    toolkit/bamtools_count.cpp
    toolkit/bamtools_coverage.cpp
    toolkit/bamtools_filter.cpp
    toolkit/bamtools_header.cpp
    toolkit/bamtools_index.cpp
    toolkit/bamtools_merge.cpp
    toolkit/bamtools_random.cpp
    toolkit/bamtools_resolve.cpp
    toolkit/bamtools_revert.cpp
    toolkit/bamtools_sort.cpp
    toolkit/bamtools_split.cpp
    toolkit/bamtools_stats.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/bamtools_version.h
)
set_target_properties(
    bamtools_cmd PROPERTIES
    OUTPUT_NAME bamtools
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF)
target_include_directories(
    bamtools_cmd PRIVATE
    ${JSONCPP_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(
    bamtools_cmd PRIVATE
    BamTools BamTools-utils ${JSONCPP_LDFLAGS})

###################
# install targets #
###################

install(
    TARGETS BamTools
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

install(
    TARGETS bamtools_cmd
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

install(
    FILES
        api/BamAlgorithms.h
        api/BamAlignment.h
        api/BamAux.h
        api/BamConstants.h
        api/BamIndex.h
        api/BamMultiReader.h
        api/BamReader.h
        api/BamWriter.h
        api/IBamIODevice.h
        api/SamConstants.h
        api/SamHeader.h
        api/SamProgram.h
        api/SamProgramChain.h
        api/SamReadGroup.h
        api/SamReadGroupDictionary.h
        api/SamSequence.h
        api/SamSequenceDictionary.h
        api/api_global.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/bamtools/api
    COMPONENT Development)
install(
    FILES
        api/algorithms/Sort.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/bamtools/api/algorithms
    COMPONENT Development)
install(
    FILES
        shared/bamtools_global.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/bamtools/shared
    COMPONENT Development)

# pkg-config
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/bamtools.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/bamtools-1.pc
    @ONLY)
install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/bamtools-1.pc
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
    COMPONENT Development)
