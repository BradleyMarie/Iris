/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisphysxtoolkit_infiniteplane.h

Abstract:

    This file contains the allocation functions for an infinite plane.

--*/

#ifndef _INFINITE_PLANE_IRIS_PHYSX_TOOLKIT_
#define _INFINITE_PLANE_IRIS_PHYSX_TOOLKIT_

#include <irisphysxtoolkit.h>

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXTOOLKITAPI
ISTATUS
PhysxInfinitePlaneAllocate(
    _In_ POINT3 Point,
    _In_ VECTOR3 SurfaceNormal,
    _In_opt_ PPBR_MATERIAL FrontMaterial,
    _In_opt_ PPBR_MATERIAL BackMaterial,
    _Out_ PPBR_GEOMETRY *Geometry
    );

#endif // _INFINITE_PLANE_IRIS_PHYSX_TOOLKIT_