/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    triangle_mesh_normal_map.h

Abstract:

    Creates a per-vertext triangle mesh normal map.

--*/

#ifndef _IRIS_PHYSX_TOOLKIT_TRIANGLE_MESH_NORMAL_MAP_
#define _IRIS_PHYSX_TOOLKIT_TRIANGLE_MESH_NORMAL_MAP_

#include "iris_physx/iris_physx.h"

#if __cplusplus 
extern "C" {
#endif // __cplusplus

//
// Functions
//

ISTATUS
TriangleMeshNormalMapAllocate(
    _In_reads_(num_vertices) const VECTOR3 normals[],
    _In_ size_t num_vertices,
    _Out_opt_ PNORMAL_MAP *normal_map
    );

#if __cplusplus 
}
#endif // __cplusplus

#endif // _IRIS_PHYSX_TOOLKIT_TRIANGLE_MESH_NORMAL_MAP_