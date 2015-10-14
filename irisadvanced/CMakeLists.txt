include(GenerateExportHeader)

include_directories(../iris/inc inc src/inc ../iriscommon)

add_library(irisadvanced SHARED
    src/visibilitytester.c
    src/random.c
    src/normal.c
    src/scene.c
    src/advancedshape.c)

target_link_libraries(irisadvanced 
    iris)

generate_export_header(irisadvanced
    BASE_NAME irisadvanced
    EXPORT_MACRO_NAME IRISADVANCEDAPI
    EXPORT_FILE_NAME inc/irisadvanced_api.h
    STATIC_DEFINE IRISADVANCED_BUILT_AS_STATIC)

set_property(TARGET irisadvanced PROPERTY C_STANDARD 11)
set_property(TARGET irisadvanced PROPERTY C_STANDARD_REQUIRED ON)