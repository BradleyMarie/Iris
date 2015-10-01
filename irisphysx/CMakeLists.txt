include(GenerateExportHeader)

include_directories(../iris/inc ../irisadvanced/inc ../irisspectrum/inc ../iriscommon inc src/inc)

add_library(irisphysx SHARED
    src/shape.c
    src/scene.c
    src/brdf.c
    src/light.c
    src/material.c)

target_link_libraries(irisphysx 
    iris
    irisadvanced
    irisspectrum)

generate_export_header(irisphysx
    BASE_NAME irisphysx
    EXPORT_MACRO_NAME IRISPHYSXAPI
    EXPORT_FILE_NAME inc/irisphysx_api.h
    STATIC_DEFINE IRISPHYSX_BUILT_AS_STATIC)