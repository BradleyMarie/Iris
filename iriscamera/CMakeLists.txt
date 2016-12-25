include(GenerateExportHeader)
include(FindOpenMP)

include_directories(../iris/inc 
    ../irisadvanced/inc 
    inc 
    ${PROJECT_BINARY_DIR}/inc
    src/inc 
    ../iriscommon)

add_library(iriscamera SHARED
    src/framebuffer.c
    src/sampletracer.c
    src/render.c
    src/pixelsampler.c
    src/camera.c
    src/raygenerator.c
    src/sampletracergenerator.c
    src/sampletracerallocator.c)

target_link_libraries(iriscamera 
    iris
    irisadvanced)

generate_export_header(iriscamera
    BASE_NAME iriscamera
    EXPORT_MACRO_NAME IRISCAMERAAPI
    EXPORT_FILE_NAME ${PROJECT_BINARY_DIR}/inc/iriscamera_api.h
    STATIC_DEFINE IRISCAMERA_BUILT_AS_STATIC)

if (OPENMP_FOUND)
	set_property(TARGET iriscamera PROPERTY C_FLAGS “${C_FLAGS} ${OpenMP_C_FLAGS}”)
	set_property(TARGET iriscamera PROPERTY C_FLAGS “${CXX_FLAGS} ${OpenMP_CXX_FLAGS}”)
endif()