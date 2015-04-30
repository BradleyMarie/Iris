include(GenerateExportHeader)

include_directories(../iris/inc ../irisadvanced/inc inc src/inc)

add_library(irisphysx SHARED
    src/reflector.c
    src/spectrum.c
    src/spectrumcompositor.c
    src/texture.c
    src/material.c
    src/shape.c
    src/scene.c)

target_link_libraries(irisphysx 
    iris
    irisadvanced)

generate_export_header(irisphysx
    BASE_NAME irisphysx
    EXPORT_MACRO_NAME IRISPHYSXAPI
    EXPORT_FILE_NAME inc/irisphysx_api.h
    STATIC_DEFINE IRISPHYSX_BUILT_AS_STATIC)