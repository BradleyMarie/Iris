include(GenerateExportHeader)

include_directories(../iris/inc 
    ../irisplusplus/inc
    ../irisadvanced/inc 
    ../irisadvancedplusplus/inc
    ../irisspectrum/inc 
    inc 
    src/inc)

add_library(irisspectrumplusplus SHARED
    src/reflector.cpp
    src/reflectorbase.cpp
    src/reflectorreference.cpp
    src/spectrum.cpp
    src/spectrumbase.cpp
    src/spectrumreference.cpp
    src/reflectorcompositor.cpp
    src/reflectorcompositorreference.cpp
    src/spectrumcompositor.cpp
    src/spectrumcompositorreference.cpp)

target_link_libraries(irisspectrumplusplus 
    iris
    irisplusplus
    irisadvanced
    irisadvancedplusplus
    irisspectrum
    irisspectrum)

generate_export_header(irisspectrumplusplus
    BASE_NAME irisspectrumplusplus
    EXPORT_MACRO_NAME IRISSPECTRUMPLUSPLUSAPI
    EXPORT_FILE_NAME inc/irisspectrumplusplus_api.h
    STATIC_DEFINE IRISSPECTRUMPLUSPLUS_BUILT_AS_STATIC)

set_property(TARGET irisspectrumplusplus PROPERTY CXX_STANDARD 11)
set_property(TARGET irisspectrumplusplus PROPERTY CXX_STANDARD_REQUIRED ON)