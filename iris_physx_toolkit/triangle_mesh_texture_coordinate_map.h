/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    triangle_mesh_texture_coordinate_map.h

Abstract:

    Creates a per-vertext triangle mesh texture_coordinate map.

--*/

#ifndef _IRIS_PHYSX_TOOLKIT_TRIANGLE_MESH_TEXTURE_COORDINATE_MAP_
#define _IRIS_PHYSX_TOOLKIT_TRIANGLE_MESH_TEXTURE_COORDINATE_MAP_

#include "iris_physx/iris_physx.h"

#if __cplusplus 
extern "C" {
#endif // __cplusplus

//
// Functions
//

ISTATUS
TriangleMeshTextureCoordinateMapAllocate(
    _In_reads_(num_vertices) const float_t texture_coordinates[][2],
    _In_ size_t num_vertices,
    _Out_ PTEXTURE_COORDINATE_MAP *texture_coordinate_map
    );

#if __cplusplus 
}
#endif // __cplusplus

#endif // _IRIS_PHYSX_TOOLKIT_TRIANGLE_MESH_TEXTURE_COORDINATE_MAP_