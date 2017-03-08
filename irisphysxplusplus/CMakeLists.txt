include(GenerateExportHeader)

include_directories(inc 
    ${PROJECT_BINARY_DIR}/inc
    src/inc
    ../iris/inc 
    ../irisadvanced/inc 
    ../irisspectrum/inc
    ../irisphysx/inc
    ../irisplusplus/inc 
    ../irisadvancedplusplus/inc 
    ../irisspectrumplusplus/inc)

add_library(irisphysxplusplus SHARED
    src/arealightbase.cpp
    src/arealightbuilder.cpp
    src/lightedgeometrybase.cpp
    src/raytracer.cpp
    src/materialbase.cpp
    src/lightbase.cpp
    src/brdfbase.cpp)

target_link_libraries(irisphysxplusplus 
    iris
    irisadvanced
    irisspectrum
    irisphysx
    irisplusplus
    irisadvancedplusplus
    irisspectrumplusplus)

generate_export_header(irisphysxplusplus
    BASE_NAME irisphysxplusplus
    EXPORT_MACRO_NAME IRISPHYSXPLUSPLUSAPI
    EXPORT_FILE_NAME ${PROJECT_BINARY_DIR}/inc/irisphysxplusplus_api.h
    STATIC_DEFINE IRISPHYSXPLUSPLUS_BUILT_AS_STATIC)