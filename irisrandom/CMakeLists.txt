include(GenerateExportHeader)

include_directories(../iris/inc inc src/inc ../iriscommon)

add_library(irisrandom SHARED
    src/randomreference.c
    src/random.c)

target_link_libraries(irisrandom 
    iris)

generate_export_header(irisrandom
    BASE_NAME irisrandom
    EXPORT_MACRO_NAME IRISRANDOMAPI
    EXPORT_FILE_NAME inc/irisrandom_api.h
    STATIC_DEFINE IRISRANDOM_BUILT_AS_STATIC)