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
PhysxSphereAllocate(
    _In_ POINT3 Center,
    _In_ FLOAT Radius,
    _In_opt_ PMATERIAL FrontMaterial,
    _In_opt_ PMATERIAL BackMaterial,
    _In_opt_ PLIGHT FrontLight,
    _In_opt_ PLIGHT BackLight,
    _Out_ PSPECTRUM_SHAPE *Shape
    );

#endif // _TRIANGLE_IRIS_PHYSX_TOOLKIT_