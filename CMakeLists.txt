cmake_minimum_required (VERSION 2.6) 
project (iris)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin) 
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin) 

add_subdirectory(iris)
add_subdirectory(irisadvanced)
add_subdirectory(irisrgb)
add_subdirectory(irisphysx)
add_subdirectory(irisshadingmodel)
add_subdirectory(iristoolkit)
add_subdirectory(irisplusplus)
add_subdirectory(test)