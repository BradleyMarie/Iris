include(GenerateExportHeader)

include_directories(../iris/inc 
    ../irisadvanced/inc 
    inc 
    ${PROJECT_BINARY_DIR}/inc
    src/inc 
    ../iriscommon)

add_library(irisadvancedtoolkit SHARED
    src/mwc.c
    src/math.c)

target_link_libraries(irisadvancedtoolkit 
    iris
    irisadvanced)

generate_export_header(irisadvancedtoolkit
    BASE_NAME irisadvancedtoolkit
    EXPORT_MACRO_NAME IRISADVANCEDTOOLKITAPI
    EXPORT_FILE_NAME ${PROJECT_BINARY_DIR}/inc/irisadvancedtoolkit_api.h
    STATIC_DEFINE IRISADVANCEDTOOLKIT_BUILT_AS_STATIC)