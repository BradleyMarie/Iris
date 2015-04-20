include(GenerateExportHeader)

include_directories(../iris/inc inc src/inc)

add_library(irisadvanced SHARED
    src/visibilitytester.c
    src/random.c
    src/normal.c
    src/scene.c
    src/boundedshape.c)

target_link_libraries(irisadvanced 
    iris)

generate_export_header(irisadvanced
    BASE_NAME irisadvanced
    EXPORT_MACRO_NAME IRISADVANCEDAPI
    EXPORT_FILE_NAME inc/irisadvanced_api.h
    STATIC_DEFINE IRISADVANCED_BUILT_AS_STATIC)