include(GenerateExportHeader)

include_directories(inc src/inc)

add_library(iris SHARED
    src/geometryhit.c
    src/matrix.c
    src/raytracer.c
    src/sceneobject.c
    src/sceneobjecttracer.c
    src/shapehitallocator.c)

generate_export_header(iris
    BASE_NAME iris
    EXPORT_MACRO_NAME IRISAPI
    EXPORT_FILE_NAME inc/iris_api.h
    STATIC_DEFINE IRIS_BUILT_AS_STATIC)