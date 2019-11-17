/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    triangle_mesh.h

Abstract:

    Creates a triangle mesh.

--*/

#ifndef _IRIS_PHYSX_TOOLKIT_TRIANGLE_MESH_
#define _IRIS_PHYSX_TOOLKIT_TRIANGLE_MESH_

#include "iris_physx/iris_physx.h"

#if __cplusplus 
extern "C" {
#endif // __cplusplus

//
// Types
//

typedef struct _TRIANGLE_MESH_ADDITIONAL_DATA {
    float_t barycentric_coordinates[3];
    uint32_t mesh_face_index;
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
    _In_opt_ PMATERIAL front_material,
    _In_opt_ PMATERIAL back_material,
    _Out_writes_(*triangles_allocated) PSHAPE shapes[],
    _Out_ size_t *triangles_allocated
    );

ISTATUS
EmissiveTriangleMeshAllocate(
    _In_reads_(num_vertices) const POINT3 vertices[],
    _In_ size_t num_vertices,
    _In_reads_(num_triangles) const size_t vertex_indices[][3],
    _In_ size_t num_triangles,
    _In_opt_ PMATERIAL front_material,
    _In_opt_ PMATERIAL back_material,
    _In_opt_ PEMISSIVE_MATERIAL front_emissive_material,
    _In_opt_ PEMISSIVE_MATERIAL back_emissive_material,
    _Out_writes_(*triangles_allocated) PSHAPE shapes[],
    _Out_ size_t *triangles_allocated
    );

#if __cplusplus 
}
#endif // __cplusplus

#endif // _IRIS_PHYSX_TOOLKIT_TRIANGLE_MESH_