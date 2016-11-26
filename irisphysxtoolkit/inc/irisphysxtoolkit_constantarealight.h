/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    irisphysxtoolkit_constantarealight.h

Abstract:

    This file contains the allocation functions for a constant area light.

--*/

#ifndef _CONSTANT_AREA_LIGHT_IRIS_PHYSX_TOOLKIT_
#define _CONSTANT_AREA_LIGHT_IRIS_PHYSX_TOOLKIT_

#include <irisphysxtoolkit.h>

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXTOOLKITAPI
ISTATUS
PhysxConstantAreaLightAllocate(
    _In_ PPHYSX_AREA_LIGHT_BUILDER Builder,
    _In_ PSPECTRUM Spectrum,
    _Out_ PSIZE_T AreaLightIndex
    );

#endif // _CONSTANT_AREA_LIGHT_IRIS_PHYSX_TOOLKIT_
