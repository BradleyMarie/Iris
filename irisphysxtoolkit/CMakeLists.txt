include(GenerateExportHeader)
include(FindOpenMP)

include_directories(inc src/inc
    ../iris/inc 
    ../irisadvanced/inc 
    ../irisadvancedtoolkit/inc 
    ../irisspectrum/inc 
    ../irisphysx/inc 
    ../iriscommon)

add_library(irisphysxtoolkit SHARED
    src/mirrorbrdf.c
    src/lambertianbrdf.c
    src/lambertianmaterial.c
    src/pointlight.c
    src/triangle.c
    src/sphere.c
    src/pinhole.c
    src/infiniteplane.c)

target_link_libraries(irisphysxtoolkit 
    iris 
    irisadvanced
    irisadvancedtoolkit
    irisspectrum
    irisphysx)

generate_export_header(irisphysxtoolkit
    BASE_NAME irisphysxtoolkit
    EXPORT_MACRO_NAME IRISPHYSXTOOLKITAPI
    EXPORT_FILE_NAME inc/irisphysxtoolkit_api.h
    STATIC_DEFINE IRISPHYSXTOOLKIT_BUILT_AS_STATIC)

option(ENABLE_CSG "Enable constructive solid geometry" OFF)

if (ENABLE_CSG)
    add_definitions(-DENABLE_CSG_SUPPORT)
endif()

if (OPENMP_FOUND)
	set_property(TARGET irisphysxtoolkit PROPERTY C_FLAGS “${C_FLAGS} ${OpenMP_C_FLAGS}”)
	set_property(TARGET irisphysxtoolkit PROPERTY C_FLAGS “${CXX_FLAGS} ${OpenMP_CXX_FLAGS}”)
endif()