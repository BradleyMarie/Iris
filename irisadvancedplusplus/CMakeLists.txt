include(GenerateExportHeader)

include_directories(../iris/inc 
    ../irisadvanced/inc 
    ../irisplusplus/inc
    inc 
    ${PROJECT_BINARY_DIR}/inc
    src/inc)

add_library(irisadvancedplusplus SHARED
    src/randombase.cpp
    src/randomgenerator.cpp)

target_link_libraries(irisadvancedplusplus 
    iris
    irisadvanced
    iriscamera
    irisplusplus)

generate_export_header(irisadvancedplusplus
    BASE_NAME irisadvancedplusplus
    EXPORT_MACRO_NAME IRISADVANCEDPLUSPLUSAPI
    EXPORT_FILE_NAME ${PROJECT_BINARY_DIR}/inc/irisadvancedplusplus_api.h
    STATIC_DEFINE IRISADVANCEDPLUSPLUS_BUILT_AS_STATIC)