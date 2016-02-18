/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisphysxtoolkit_lambertianbrdf.h

Abstract:

    This file contains the allocation functions for a lambertian brdf.

--*/

#ifndef _LAMBERTIAN_BRDF_IRIS_PHYSX_TOOLKIT_
#define _LAMBERTIAN_BRDF_IRIS_PHYSX_TOOLKIT_

#include <irisphysxtoolkit.h>

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXTOOLKITAPI
ISTATUS
SpectrumLambertianBrdfAllocate(
    _In_ PBRDF_ALLOCATOR Allocator,
    _In_ PCREFLECTOR Reflectance,
    _In_ VECTOR3 SurfaceNormal,
    _Out_ PCBRDF *Brdf
    );

#endif // _LAMBERTIAN_BRDF_IRIS_PHYSX_TOOLKIT_