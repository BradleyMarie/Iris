/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    quad.h

Abstract:

    Allocates two triangles from the vertices of a quadrilateral.

--*/

#ifndef _TEST_UTIL_QUAD_
#define _TEST_UTIL_QUAD_

#include "iris_physx/iris_physx.h"

#if __cplusplus 
extern "C" {
#endif // __cplusplus

//
// Functions
//

ISTATUS
EmissiveQuadAllocate(
    _In_ POINT3 v0,
    _In_ POINT3 v1,
    _In_ POINT3 v2,
    _In_ POINT3 v3,
    _In_opt_ PMATERIAL front_material,
    _In_opt_ PMATERIAL back_material,
    _In_opt_ PEMISSIVE_MATERIAL front_emissive_material,
    _In_opt_ PEMISSIVE_MATERIAL back_emissive_material,
    _Out_ PSHAPE *shape0,
    _Out_ PSHAPE *shape1
    );

#if __cplusplus 
}
#endif // __cplusplus

#endif // _TEST_UTIL_QUAD_