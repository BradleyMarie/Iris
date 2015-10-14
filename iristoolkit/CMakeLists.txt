include(GenerateExportHeader)

include_directories(../iris/inc ../irisshadingmodel/inc inc src/inc)

add_library(iristoolkit SHARED
    src/triangle.c
    src/sphere.c
    src/listscene.c
    src/tracer.c
    src/framebuffer.c
    src/constantnormal.c
    src/constantemissiveshader.c
    src/camera.c
    src/interpolatedtriangle.c
    src/lightshader.c
    src/phong.c
    src/spherenormal.c
    src/perfectspecular.c
    src/constanttexture.c
    src/infiniteplane.c
    src/mwc.c
    src/lambertianindirect.c)

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

set_property(TARGET iristoolkit PROPERTY C_STANDARD 11)
set_property(TARGET iristoolkit PROPERTY C_STANDARD_REQUIRED ON)