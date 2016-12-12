include(GenerateExportHeader)

include_directories(../iris/inc 
    ../irisadvanced/inc 
    inc 
    ${PROJECT_BINARY_DIR}/inc
    src/inc 
    ../iriscommon)

add_library(iriscamera SHARED
    src/randomgenerator.c
    src/framebuffer.c)

target_link_libraries(iriscamera 
    iris
    irisadvanced)

generate_export_header(iriscamera
    BASE_NAME iriscamera
    EXPORT_MACRO_NAME IRISCAMERAAPI
    EXPORT_FILE_NAME ${PROJECT_BINARY_DIR}/inc/iriscamera_api.h
    STATIC_DEFINE IRISCAMERA_BUILT_AS_STATIC)