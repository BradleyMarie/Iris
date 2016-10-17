/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisphysxtoolkit_mirrorbrdf.h

Abstract:

    This file contains the allocation functions for a mirror brdf.

--*/

#ifndef _MIRROR_BRDF_IRIS_PHYSX_TOOLKIT_
#define _MIRROR_BRDF_IRIS_PHYSX_TOOLKIT_

#include <irisphysxtoolkit.h>

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXTOOLKITAPI
ISTATUS
SpectrumMirrorBrdfAllocate(
    _In_ PPHYSX_BRDF_ALLOCATOR Allocator,
    _In_ PCREFLECTOR Reflectance,
    _Out_ PCPHYSX_BRDF *Brdf
    );

#endif // _MIRROR_BRDF_IRIS_PHYSX_TOOLKIT_
