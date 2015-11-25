include(GenerateExportHeader)

include_directories(../iris/inc ../irisadvanced/inc ../irisspectrum/inc ../irisphysx/inc inc src/inc)

add_library(irisphysxtoolkit SHARED
    src/lighting.c
    src/pointlight.c
    src/mirrorbrdf.c
    src/lambertianbrdf.c)

target_link_libraries(irisphysxtoolkit 
    iris 
    irisadvanced
    irisspectrum
    irisphysx)

generate_export_header(irisphysxtoolkit
    BASE_NAME irisphysxtoolkit
    EXPORT_MACRO_NAME IRISPHYSXTOOLKITAPI
    EXPORT_FILE_NAME inc/irisphysxtoolkit_api.h
    STATIC_DEFINE IRISPHYSXTOOLKIT_BUILT_AS_STATIC)

option(ENABLE_CSG "Enable constructive solid geometry" OFF)

if (ENABLE_CSG)
    add_definitions(-DENABLE_CSG_SUPPORT)
endif()

set_property(TARGET irisphysxtoolkit PROPERTY C_STANDARD 11)
set_property(TARGET irisphysxtoolkit PROPERTY C_STANDARD_REQUIRED ON)