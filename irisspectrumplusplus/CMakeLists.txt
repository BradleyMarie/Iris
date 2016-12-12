include(GenerateExportHeader)

include_directories(../iris/inc 
    ../irisplusplus/inc
    ../irisspectrum/inc 
    inc 
    ${PROJECT_BINARY_DIR}/inc
    src/inc)

add_library(irisspectrumplusplus SHARED
    src/reflectorbase.cpp
    src/spectrumbase.cpp)

target_link_libraries(irisspectrumplusplus 
    iris
    irisspectrum
    irisplusplus)

generate_export_header(irisspectrumplusplus
    BASE_NAME irisspectrumplusplus
    EXPORT_MACRO_NAME IRISSPECTRUMPLUSPLUSAPI
    EXPORT_FILE_NAME ${PROJECT_BINARY_DIR}/inc/irisspectrumplusplus_api.h
    STATIC_DEFINE IRISSPECTRUMPLUSPLUS_BUILT_AS_STATIC)