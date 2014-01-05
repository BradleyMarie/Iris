cmake_minimum_required (VERSION 2.6) 
project (iris) 

include_directories(inc src/inc)

add_library(iris SHARED
    src/geometryhit.c
    src/matrix.c
    src/raytracer.c
    src/sceneobject.c
    src/sceneobjecttracer.c
    src/shapehitallocator.c)
    