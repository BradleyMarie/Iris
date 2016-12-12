include(GenerateExportHeader)

include_directories(../iris/inc 
    inc 
    ${PROJECT_BINARY_DIR}/inc
    src/inc 
    ../iriscommon)

add_library(irisadvanced SHARED
    src/randomreference.c
    src/random.c)

target_link_libraries(irisadvanced 
    iris)

generate_export_header(irisadvanced
    BASE_NAME irisadvanced
    EXPORT_MACRO_NAME IRISADVANCEDAPI
    EXPORT_FILE_NAME ${PROJECT_BINARY_DIR}/inc/irisadvanced_api.h
    STATIC_DEFINE IRISADVANCED_BUILT_AS_STATIC)