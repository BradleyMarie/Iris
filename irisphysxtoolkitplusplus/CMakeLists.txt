include(GenerateExportHeader)

include_directories(inc src/inc
    ../iris/inc 
    ../irisadvanced/inc 
    ../irisspectrum/inc
    ../irisphysx/inc
    ../irisplusplus/inc 
    ../irisadvancedplusplus/inc 
    ../irisspectrumplusplus/inc
    ../irisphysxplusplus/inc
    ../irisadvancedtoolkit/inc
    ../irisadvancedtoolkitplusplus/inc
    ../irisphysxtoolkit/inc)

add_library(irisphysxtoolkitplusplus SHARED
    src/pinhole.cpp)

target_link_libraries(irisphysxtoolkitplusplus 
    iris
    irisadvanced
    irisspectrum
    irisphysx
    irisplusplus
    irisadvancedplusplus
    irisspectrumplusplus
    irisphysxplusplus
    irisadvancedtoolkit
    irisphysxtoolkit)

generate_export_header(irisphysxtoolkitplusplus
    BASE_NAME irisphysxtoolkitplusplus
    EXPORT_MACRO_NAME IRISPHYSXTOOLKITPLUSPLUSAPI
    EXPORT_FILE_NAME inc/irisphysxtoolkitplusplus_api.h
    STATIC_DEFINE IRISPHYSXTOOLKITPLUSPLUS_BUILT_AS_STATIC)