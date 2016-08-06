/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisphysxtoolkit_sphere.h

Abstract:

    This file contains the allocation functions for a sphere.

--*/

#ifndef _SPHERE_IRIS_PHYSX_TOOLKIT_
#define _SPHERE_IRIS_PHYSX_TOOLKIT_

#include <irisphysxtoolkit.h>

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXTOOLKITAPI
ISTATUS
PhysxSphereAllocate(
    _In_ POINT3 Center,
    _In_ FLOAT Radius,
    _In_opt_ PPBR_MATERIAL FrontMaterial,
    _In_opt_ PPBR_MATERIAL BackMaterial,
    _Out_ PPBR_GEOMETRY *Geometry
    );

#endif // _SPHERE_IRIS_PHYSX_TOOLKIT_