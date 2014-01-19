include_directories(../iris/inc inc src/inc)

add_library(iristoolkit SHARED
    src/triangle.c)

target_link_libraries(iristoolkit 
    iris)