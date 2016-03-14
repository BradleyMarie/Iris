include(GenerateExportHeader)

include_directories(../iris/inc ../irisadvanced/inc ../iriscommon inc src/inc)

add_library(irisspectrum SHARED
    src/spectrumreference.c
    src/spectrum.c
    src/reflectorreference.c
    src/reflector.c
    src/spectrumcompositor.c
    src/reflectorcompositor.c)

target_link_libraries(irisspectrum 
    iris
    irisadvanced)

generate_export_header(irisspectrum
    BASE_NAME irisspectrum
    EXPORT_MACRO_NAME IRISSPECTRUMAPI
    EXPORT_FILE_NAME inc/irisspectrum_api.h
    STATIC_DEFINE IRISSPECTRUM_BUILT_AS_STATIC)

set_property(TARGET irisspectrum PROPERTY C_STANDARD 11)
set_property(TARGET irisspectrum PROPERTY C_STANDARD_REQUIRED ON)