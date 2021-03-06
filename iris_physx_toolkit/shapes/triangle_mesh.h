/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    triangle_mesh.h

Abstract:

    Creates a triangle mesh.

--*/

#ifndef _IRIS_PHYSX_TOOLKIT_SHAPES_TRIANGLE_MESH_
#define _IRIS_PHYSX_TOOLKIT_SHAPES_TRIANGLE_MESH_

#include "iris_physx/iris_physx.h"

#if __cplusplus 
extern "C" {
#endif // __cplusplus

//
// Types
//

typedef struct _TRIANGLE_MESH_ADDITIONAL_DATA {
    float_t barycentric_coordinates[3];
    size_t vertex_indices[3];
    PCPOINT3 vertices;
} TRIANGLE_MESH_ADDITIONAL_DATA, *PTRIANGLE_MESH_ADDITIONAL_DATA;

typedef const TRIANGLE_MESH_ADDITIONAL_DATA *PCTRIANGLE_MESH_ADDITIONAL_DATA;

//
// Constants
//

#define TRIANGLE_MESH_FRONT_FACE 0
#define TRIANGLE_MESH_BACK_FACE  1

//
// Functions
//

ISTATUS
TriangleMeshAllocate(
    _In_reads_(num_vertices) const POINT3 vertices[],
    _In_ size_t num_vertices,
    _In_reads_(num_triangles) const size_t vertex_indices[][3],
    _In_ size_t num_triangles,
    _In_opt_ PTEXTURE_COORDINATE_MAP front_texture_coordinate_map,
    _In_opt_ PTEXTURE_COORDINATE_MAP back_texture_coordinate_map,
    _In_opt_ PNORMAL_MAP front_normal_map,
    _In_opt_ PNORMAL_MAP back_normal_map,
    _In_opt_ PMATERIAL front_material,
    _In_opt_ PMATERIAL back_material,
    _In_opt_ PEMISSIVE_MATERIAL front_emissive_material,
    _In_opt_ PEMISSIVE_MATERIAL back_emissive_material,
    _Out_writes_(num_triangles) PSHAPE shapes[],
    _Out_ size_t *triangles_allocated
    );

#if __cplusplus 
}
#endif // __cplusplus

#endif // _IRIS_PHYSX_TOOLKIT_SHAPES_TRIANGLE_MESH_