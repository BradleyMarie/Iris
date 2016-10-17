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
    _In_opt_ PPBR_MATERIAL FrontMaterial,
    _In_opt_ PPBR_MATERIAL BackMaterial,
    _Out_ PPHYSX_GEOMETRY *Geometry
    );

#endif // _TRIANGLE_IRIS_PHYSX_TOOLKIT_
