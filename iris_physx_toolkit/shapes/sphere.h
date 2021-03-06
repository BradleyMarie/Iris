/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    sphere.h

Abstract:

    Creates a sphere.

--*/

#ifndef _IRIS_PHYSX_TOOLKIT_SHAPES_SPHERE_
#define _IRIS_PHYSX_TOOLKIT_SHAPES_SPHERE_

#include "iris_physx/iris_physx.h"

#if __cplusplus 
extern "C" {
#endif // __cplusplus

//
// Constants
//

#define SPHERE_FRONT_FACE 0
#define SPHERE_BACK_FACE  1

//
// Functions
//

ISTATUS
SphereAllocate(
    _In_ POINT3 center,
    _In_ float_t radius,
    _In_opt_ PMATERIAL front_material,
    _In_opt_ PMATERIAL back_material,
    _Out_ PSHAPE *shape
    );

ISTATUS
EmissiveSphereAllocate(
    _In_ POINT3 center,
    _In_ float_t radius,
    _In_opt_ PMATERIAL front_material,
    _In_opt_ PMATERIAL back_material,
    _In_opt_ PEMISSIVE_MATERIAL front_emissive_material,
    _In_opt_ PEMISSIVE_MATERIAL back_emissive_material,
    _Out_ PSHAPE *shape
    );

#if __cplusplus 
}
#endif // __cplusplus

#endif // _IRIS_PHYSX_TOOLKIT_SHAPES_SPHERE_