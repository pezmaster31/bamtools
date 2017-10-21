# ==========================
# BamTools CMakeLists.txt
# (c) 2010 Derek Barnett
#
# src/third-party/jsoncpp
# ==========================

# add compiler definitions 
add_definitions( -DBAMTOOLS_JSONCPP_LIBRARY ) # (for proper exporting of library symbols)

# create jsoncpp library
add_library( jsoncpp STATIC
             json_reader.cpp
             json_value.cpp
             json_writer.cpp
           )

# set jsoncpp library properties
set_target_properties( jsoncpp PROPERTIES
                       OUTPUT_NAME jsoncpp
                       PREFIX "lib"
                     )
