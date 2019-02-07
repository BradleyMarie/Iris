/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    constructive_solid_geometry.h

Abstract:

    Creates aggregate shapes for doing constructive solid geometry.

--*/

#ifndef _IRIS_PHYSX_TOOLKIT_CONSTRUCTIVE_SOLID_GEOMETRY_
#define _IRIS_PHYSX_TOOLKIT_CONSTRUCTIVE_SOLID_GEOMETRY_

#include "iris_physx/iris_physx.h"

#if __cplusplus
extern "C" {
#endif // __cplusplus

//
// Functions
//

ISTATUS
DifferenceAllocate(
    _In_ PSHAPE minuend,
    _In_ PSHAPE subtrahend,
    _Out_ PSHAPE *result
    );

ISTATUS
IntersectionAllocate(
    _In_ PSHAPE shape0,
    _In_ PSHAPE shape1,
    _Out_ PSHAPE *result
    );

ISTATUS
UnionAllocate(
    _In_ PSHAPE shape0,
    _In_ PSHAPE shape1,
    _Out_ PSHAPE *result
    );

#if __cplusplus
}
#endif // __cplusplus

#endif // _IRIS_PHYSX_TOOLKIT_CONSTRUCTIVE_SOLID_GEOMETRY_