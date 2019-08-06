/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    triangle.h

Abstract:

    Creates a triangle.

--*/

#ifndef _IRIS_PHYSX_TOOLKIT_TRIANGLE_
#define _IRIS_PHYSX_TOOLKIT_TRIANGLE_

#include "iris_physx/iris_physx.h"

#if __cplusplus 
extern "C" {
#endif // __cplusplus

//
// Types
//

typedef struct _TRIANGLE_ADDITIONAL_DATA {
    float_t barycentric_coordinates[3];
} TRIANGLE_ADDITIONAL_DATA, *PTRIANGLE_ADDITIONAL_DATA;

typedef const TRIANGLE_ADDITIONAL_DATA *PCTRIANGLE_ADDITIONAL_DATA;

//
// Constants
//

#define TRIANGLE_FRONT_FACE 0
#define TRIANGLE_BACK_FACE  1

//
// Functions
//

ISTATUS
TriangleAllocate(
    _In_ POINT3 v0,
    _In_ POINT3 v1,
    _In_ POINT3 v2,
    _In_opt_ PMATERIAL front_material,
    _In_opt_ PMATERIAL back_material,
    _Out_ PSHAPE *shape
    );

ISTATUS
EmissiveTriangleAllocate(
    _In_ POINT3 v0,
    _In_ POINT3 v1,
    _In_ POINT3 v2,
    _In_opt_ PMATERIAL front_material,
    _In_opt_ PMATERIAL back_material,
    _In_opt_ PEMISSIVE_MATERIAL front_emissive_material,
    _In_opt_ PEMISSIVE_MATERIAL back_emissive_material,
    _Out_ PSHAPE *shape
    );

#if __cplusplus 
}
#endif // __cplusplus

#endif // _IRIS_PHYSX_TOOLKIT_TRIANGLE_