include(GenerateExportHeader)

include_directories(../iris/inc
    inc 
    src/inc)

add_library(irisplusplus SHARED
    src/utility.cpp
    src/matrix.cpp
    src/hittester.cpp
    src/raytracer.cpp)

target_link_libraries(irisplusplus 
    iris)

generate_export_header(irisplusplus
    BASE_NAME irisplusplus
    EXPORT_MACRO_NAME IRISPLUSPLUSAPI
    EXPORT_FILE_NAME inc/irisplusplus_api.h
    STATIC_DEFINE IRISPLUSPLUS_BUILT_AS_STATIC)