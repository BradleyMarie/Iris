/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisphysxtoolkit_triangle.h

Abstract:

    This file contains the allocation functions for a triangle.

--*/

#ifndef _TRIANGLE_IRIS_PHYSX_TOOLKIT_
#define _TRIANGLE_IRIS_PHYSX_TOOLKIT_

#include <irisphysxtoolkit.h>

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXTOOLKITAPI
ISTATUS
PhysxTriangleAllocate(
    _In_ POINT3 Vertex0,
    _In_ POINT3 Vertex1,
    _In_ POINT3 Vertex2,
    _In_opt_ PPHYSX_MATERIAL FrontMaterial,
    _In_opt_ PPHYSX_MATERIAL BackMaterial,
    _Out_ PPHYSX_GEOMETRY *Geometry
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXTOOLKITAPI
ISTATUS
PhysxLightedTriangleAllocate(
    _In_ PPHYSX_AREA_LIGHT_BUILDER Builder,
    _In_ POINT3 Vertex0,
    _In_ POINT3 Vertex1,
    _In_ POINT3 Vertex2,
    _In_opt_ PPHYSX_MATERIAL FrontMaterial,
    _In_opt_ PPHYSX_MATERIAL BackMaterial,
    _Out_ PSIZE_T AreaGeometryIndex
    );

#endif // _TRIANGLE_IRIS_PHYSX_TOOLKIT_
