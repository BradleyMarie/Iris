include(GenerateExportHeader)

include_directories(../iris/inc inc src/inc ../iriscommon)

add_library(irisshadingmodel SHARED
    src/surfacenormal.c
    src/scene.c
    src/sceneobject.c
    src/scenetracer.c
    src/visibilitytester.c
    src/rayshader.c
    src/textureshader.c
    src/texture.c
    src/translucentshader.c
    src/drawingshape.c
    src/random.c
    src/normal.c
    src/emissive.c
    src/indirect.c
    src/direct.c)

target_link_libraries(irisshadingmodel 
    iris)

generate_export_header(irisshadingmodel
    BASE_NAME irisshadingmodel
    EXPORT_MACRO_NAME IRISSHADINGMODELAPI
    EXPORT_FILE_NAME inc/irisshadingmodel_api.h
    STATIC_DEFINE IRISSHADINGMODEL_BUILT_AS_STATIC)

set_property(TARGET irisshadingmodel PROPERTY C_STANDARD 11)
set_property(TARGET irisshadingmodel PROPERTY C_STANDARD_REQUIRED ON)