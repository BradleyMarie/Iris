include(GenerateExportHeader)

include_directories(../iris/inc ../irisadvanced/inc inc src/inc)

add_library(irisrgb SHARED
    src/surfacenormal.c
    src/rgbscene.c
    src/rgbshape.c
    src/rayshader.c
    src/textureshader.c
    src/texture.c
    src/translucentshader.c
    src/emissive.c
    src/indirect.c
    src/direct.c)

target_link_libraries(irisrgb 
    iris
    irisadvanced)

generate_export_header(irisrgb
    BASE_NAME irisrgb
    EXPORT_MACRO_NAME IRISRGBAPI
    EXPORT_FILE_NAME inc/irisrgb_api.h
    STATIC_DEFINE IRISRGB_BUILT_AS_STATIC)