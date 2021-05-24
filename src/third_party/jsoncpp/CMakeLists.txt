add_library(
    jsoncpp
    STATIC
    jsoncpp.cpp
)
set_target_properties(
    jsoncpp PROPERTIES
    POSITION_INDEPENDENT_CODE ${BUILD_SHARED_LIBS})

if(MSVC)
    target_compile_options(
        jsoncpp PRIVATE
        /EHsc)
endif()
