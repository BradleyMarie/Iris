include(GenerateExportHeader)

include_directories(../iris/inc ../irisadvanced/inc ../irisspectrum/inc ../iriscommon inc src/inc)

add_library(irisphysx SHARED
    src/brdf.c
    src/light.c
    src/material.c
    src/brdfallocator.c
    src/hitallocator.c
    src/geometry.c
    src/hittester.c
    src/raytraceradapter.c
    src/visibilitytester.c)

target_link_libraries(irisphysx 
    iris
    irisadvanced
    irisspectrum)

generate_export_header(irisphysx
    BASE_NAME irisphysx
    EXPORT_MACRO_NAME IRISPHYSXAPI
    EXPORT_FILE_NAME inc/irisphysx_api.h
    STATIC_DEFINE IRISPHYSX_BUILT_AS_STATIC)

set_property(TARGET irisphysx PROPERTY C_STANDARD 11)
set_property(TARGET irisphysx PROPERTY C_STANDARD_REQUIRED ON)