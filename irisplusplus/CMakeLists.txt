include(GenerateExportHeader)

include_directories(../iris/inc 
    ../irisshadingmodel/inc 
    ../iristoolkit/inc
    inc 
    src/inc)

add_library(irisplusplus SHARED
    src/shape.cpp
    src/matrix.cpp
    src/shapehitallocator.cpp
    src/normal.cpp
    src/texture.cpp
    src/translucentshader.cpp
    src/emissiveshader.cpp
    src/random.cpp)

target_link_libraries(irisplusplus
    iristoolkit 
    iris 
    irisshadingmodel)

generate_export_header(irisplusplus
    BASE_NAME irisplusplus
    EXPORT_MACRO_NAME IRISPLUSPLUSAPI
    EXPORT_FILE_NAME inc/irisplusplus_api.h
    STATIC_DEFINE IRISPLUSPLUS_BUILT_AS_STATIC)

option(ENABLE_CSG "Enable constructive solid geometry" OFF)

if (ENABLE_CSG)
    add_definitions(-DENABLE_CSG_SUPPORT)
endif()