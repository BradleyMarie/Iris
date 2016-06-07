include(GenerateExportHeader)

include_directories(inc src/inc
    ../iris/inc 
    ../irisadvanced/inc 
    ../irisadvancedtoolkit/inc 
    ../irisshadingmodel/inc 
    ../iriscommon)

add_library(iristoolkit SHARED
    src/triangle.c
    src/sphere.c
    src/listscene.c
    src/tracer.c
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
    src/lambertianindirect.c)

target_link_libraries(iristoolkit 
    iris 
    irisadvanced
    irisadvancedtoolkit
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