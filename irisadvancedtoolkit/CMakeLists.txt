include(GenerateExportHeader)

include_directories(../iris/inc 
    ../irisrandom/inc 
    ../irisadvanced/inc 
    ../iriscommon 
    inc 
    src/inc)

add_library(irisadvancedtoolkit SHARED
    src/framebuffer.c
    src/mwc.c)

target_link_libraries(irisadvancedtoolkit 
    iris
    irisrandom
    irisadvanced)

generate_export_header(irisadvancedtoolkit
    BASE_NAME irisadvancedtoolkit
    EXPORT_MACRO_NAME IRISADVANCEDTOOLKITAPI
    EXPORT_FILE_NAME inc/irisadvancedtoolkit_api.h
    STATIC_DEFINE IRISADVANCEDTOOLKIT_BUILT_AS_STATIC)