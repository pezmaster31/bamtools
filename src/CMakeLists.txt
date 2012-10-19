# ==========================
# BamTools CMakeLists.txt
# (c) 2010 Derek Barnett
#
# src/
# ==========================

add_subdirectory( api )
add_subdirectory( third_party )
add_subdirectory( toolkit )
add_subdirectory( utils )

# export shared headers
install( FILES shared/bamtools_global.h DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/bamtools/shared" COMPONENT Development )

# configure and install pkg-config file
configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/bamtools.pc.in ${CMAKE_CURRENT_BINARY_DIR}/bamtools-1.pc @ONLY )
install( FILES ${CMAKE_CURRENT_BINARY_DIR}/bamtools-1.pc DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig )
