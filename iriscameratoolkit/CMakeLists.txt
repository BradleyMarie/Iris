include(GenerateExportHeader)

include_directories(../iris/inc 
    ../irisadvanced/inc 
    ../iriscamera/inc 
    ../irisadvancedtoolkit/inc 
    inc 
    ${PROJECT_BINARY_DIR}/inc
    src/inc 
    ../iriscommon)

add_library(iriscameratoolkit SHARED
    src/pinhole.c
    src/gridpixelsampler.c)

target_link_libraries(iriscameratoolkit
    iris
    irisadvanced
    iriscamera
    irisadvancedtoolkit)

generate_export_header(iriscameratoolkit
    BASE_NAME iriscameratoolkit
    EXPORT_MACRO_NAME IRISCAMERATOOLKITAPI
    EXPORT_FILE_NAME ${PROJECT_BINARY_DIR}/inc/iriscameratoolkit_api.h
    STATIC_DEFINE IRISCAMERATOOLKIT_BUILT_AS_STATIC)