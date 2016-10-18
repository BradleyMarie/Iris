/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisphysxtoolkit_pointlight.h

Abstract:

    This file contains the allocation functions for a point light.

--*/

#ifndef _POINT_LIGHT_IRIS_PHYSX_TOOLKIT_
#define _POINT_LIGHT_IRIS_PHYSX_TOOLKIT_

#include <irisphysxtoolkit.h>

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXTOOLKITAPI
ISTATUS
SpectrumPointLightAllocate(
    _In_ PSPECTRUM Intensity,
    _In_ POINT3 WorldLocation,
    _Out_ PPHYSX_LIGHT *Light
    );

#endif // _POINT_LIGHT_IRIS_PHYSX_TOOLKIT_
