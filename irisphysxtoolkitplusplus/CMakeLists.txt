include(GenerateExportHeader)

include_directories(inc 
    ${PROJECT_BINARY_DIR}/inc
    src/inc
    ../iris/inc 
    ../irisadvanced/inc 
    ../iriscamera/inc 
    ../irisspectrum/inc
    ../irisphysx/inc
    ../irisplusplus/inc 
    ../irisadvancedplusplus/inc 
    ../iriscameraplusplus/inc 
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
    iriscamera
    irisspectrum
    irisphysx
    irisplusplus
    irisadvancedplusplus
    irisspectrumplusplus
    irisphysxplusplus
    irisphysxtoolkit)

generate_export_header(irisphysxtoolkitplusplus
    BASE_NAME irisphysxtoolkitplusplus
    EXPORT_MACRO_NAME IRISPHYSXTOOLKITPLUSPLUSAPI
    EXPORT_FILE_NAME ${PROJECT_BINARY_DIR}/inc/irisphysxtoolkitplusplus_api.h
    STATIC_DEFINE IRISPHYSXTOOLKITPLUSPLUS_BUILT_AS_STATIC)