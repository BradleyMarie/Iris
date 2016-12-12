include(GenerateExportHeader)

include_directories(../iris/inc 
    ../irisrandom/inc 
    inc 
    ${PROJECT_BINARY_DIR}/inc
    src/inc 
    ../iriscommon)

add_library(irisrandomtoolkit SHARED
    src/mwc.c
    src/math.c)

target_link_libraries(irisrandomtoolkit 
    iris
    irisrandom)

generate_export_header(irisrandomtoolkit
    BASE_NAME irisrandomtoolkit
    EXPORT_MACRO_NAME IRISRANDOMTOOLKITAPI
    EXPORT_FILE_NAME ${PROJECT_BINARY_DIR}/inc/irisrandomtoolkit_api.h
    STATIC_DEFINE IRISRANDOMTOOLKIT_BUILT_AS_STATIC)