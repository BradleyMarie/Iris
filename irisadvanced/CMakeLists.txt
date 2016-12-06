include(GenerateExportHeader)

include_directories(../iris/inc 
    ../irisrandom/inc 
    inc 
    src/inc 
    ../iriscommon)

add_library(irisadvanced SHARED
    src/randomgenerator.c
    src/framebuffer.c
    src/math.c)

target_link_libraries(irisadvanced 
    iris
    irisrandom)

generate_export_header(irisadvanced
    BASE_NAME irisadvanced
    EXPORT_MACRO_NAME IRISADVANCEDAPI
    EXPORT_FILE_NAME inc/irisadvanced_api.h
    STATIC_DEFINE IRISADVANCED_BUILT_AS_STATIC)