include(GenerateExportHeader)

include_directories(../iris/inc ../irisshadingmodel/inc inc src/inc)

add_library(iristoolkit SHARED
    src/triangle.c
    src/sphere.c
    src/listscene.c
    src/tracer.c
    src/framebuffer.c
    src/constantnormal.c
    src/constantemissiveshader.c)

target_link_libraries(iristoolkit 
    iris 
    irisshadingmodel)

generate_export_header(iristoolkit
    BASE_NAME iristoolkit
    EXPORT_MACRO_NAME IRISTOOLKITAPI
    EXPORT_FILE_NAME inc/iristoolkit_api.h
    STATIC_DEFINE IRISTOOLKIT_BUILT_AS_STATIC)

option(ENABLE_CSG "Enable constructive solid geometry" OFF)

if (ENABLE_CSG)
    add_definitions(-DENABLE_CSG_SUPPORT)
endif()