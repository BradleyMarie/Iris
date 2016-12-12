include(GenerateExportHeader)

include_directories(inc 
    ${PROJECT_BINARY_DIR}/inc
    src/inc
    ../iris/inc 
    ../irisrandom/inc 
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
    irisrandom
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
    EXPORT_FILE_NAME ${PROJECT_BINARY_DIR}/inc/irisphysxtoolkitplusplus_api.h
    STATIC_DEFINE IRISPHYSXTOOLKITPLUSPLUS_BUILT_AS_STATIC)