include(GenerateExportHeader)

include_directories(../iris/inc
    inc 
    src/inc)

add_library(irisplusplus SHARED
    src/utility.cpp
    src/shape.cpp
    src/shapebase.cpp
    src/matrix.cpp
    src/matrixreference.cpp
    src/hittester.cpp
    src/raytracer.cpp)

target_link_libraries(irisplusplus 
    iris)

generate_export_header(irisplusplus
    BASE_NAME irisplusplus
    EXPORT_MACRO_NAME IRISPLUSPLUSAPI
    EXPORT_FILE_NAME inc/irisplusplus_api.h
    STATIC_DEFINE IRISPLUSPLUS_BUILT_AS_STATIC)

set_property(TARGET irisplusplus PROPERTY CXX_STANDARD 11)
set_property(TARGET irisplusplus PROPERTY CXX_STANDARD_REQUIRED ON)