include(GenerateExportHeader)

include_directories(inc src/inc ../iriscommon)

add_library(iris SHARED
    src/matrixreference.c
    src/matrix.c
    src/raytracer.c
    src/scene.c
    src/shapehitallocator.c
    src/point.c
    src/vector.c
    src/ray.c
    src/shape.c)

generate_export_header(iris
    BASE_NAME iris
    EXPORT_MACRO_NAME IRISAPI
    EXPORT_FILE_NAME inc/iris_api.h
    STATIC_DEFINE IRIS_BUILT_AS_STATIC)

set_property(TARGET iris PROPERTY C_STANDARD 11)
set_property(TARGET iris PROPERTY C_STANDARD_REQUIRED ON)