include(GenerateExportHeader)

include_directories(../iris/inc 
    inc 
    ${PROJECT_BINARY_DIR}/inc
    src/inc 
    ../iriscommon)

add_library(irisrandom SHARED
    src/randomreference.c
    src/random.c)

target_link_libraries(irisrandom 
    iris)

generate_export_header(irisrandom
    BASE_NAME irisrandom
    EXPORT_MACRO_NAME IRISRANDOMAPI
    EXPORT_FILE_NAME ${PROJECT_BINARY_DIR}/inc/irisrandom_api.h
    STATIC_DEFINE IRISRANDOM_BUILT_AS_STATIC)