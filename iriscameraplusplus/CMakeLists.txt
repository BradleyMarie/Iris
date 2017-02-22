include(GenerateExportHeader)

include_directories(../iris/inc 
    ../irisadvanced/inc 
    ../iriscamera/inc 
    ../irisadvancedplusplus/inc 
    ../irisplusplus/inc 
    inc 
    ${PROJECT_BINARY_DIR}/inc
    src/inc 
    ../iriscommon)

add_library(iriscameraplusplus SHARED
    src/sampletracerbase.cpp)

target_link_libraries(iriscameraplusplus 
    iris
    irisadvanced
    iriscamera
    irisplusplus
    irisadvancedplusplus)

generate_export_header(iriscameraplusplus
    BASE_NAME iriscameraplusplus
    EXPORT_MACRO_NAME IRISCAMERAPLUSPLUSAPI
    EXPORT_FILE_NAME ${PROJECT_BINARY_DIR}/inc/iriscameraplusplus_api.h
    STATIC_DEFINE IRISCAMERAPLUSPLUS_BUILT_AS_STATIC)