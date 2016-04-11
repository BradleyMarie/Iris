include(GenerateExportHeader)

include_directories(../iris/inc 
    ../irisadvanced/inc 
    ../irisplusplus/inc
    inc 
    src/inc)

add_library(irisadvancedplusplus SHARED
    src/randombase.cpp)

target_link_libraries(irisadvancedplusplus 
    iris
    irisadvanced
    irisplusplus)

generate_export_header(irisadvancedplusplus
    BASE_NAME irisadvancedplusplus
    EXPORT_MACRO_NAME IRISADVANCEDPLUSPLUSAPI
    EXPORT_FILE_NAME inc/irisadvancedplusplus_api.h
    STATIC_DEFINE IRISADVANCEDPLUSPLUS_BUILT_AS_STATIC)

set_property(TARGET irisadvancedplusplus PROPERTY CXX_STANDARD 11)
set_property(TARGET irisadvancedplusplus PROPERTY CXX_STANDARD_REQUIRED ON)