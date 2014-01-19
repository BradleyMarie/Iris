include(GenerateExportHeader)

include_directories(../iris/inc inc src/inc)

add_library(iristoolkit SHARED
    src/triangle.c)

target_link_libraries(iristoolkit 
    iris)

generate_export_header(iristoolkit
    BASE_NAME iristoolkit
    EXPORT_MACRO_NAME IRISTOOLKITAPI
    EXPORT_FILE_NAME inc/iristoolkit_api.h
    STATIC_DEFINE IRISTOOLKIT_BUILT_AS_STATIC)