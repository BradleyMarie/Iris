/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    lambertian_brdf.h

Abstract:

    Creates a Lambertian BRDF.

--*/

#ifndef _IRIS_PHYSX_TOOLKIT_LAMBERTIAN_BRDF_
#define _IRIS_PHYSX_TOOLKIT_LAMBERTIAN_BRDF_

#include "iris_physx/iris_physx.h"

#if __cplusplus 
extern "C" {
#endif // __cplusplus

//
// Functions
//

ISTATUS
LambertianBrdfAllocate(
    _In_ PREFLECTOR reflector,
    _Out_ PBRDF *brdf
    );

ISTATUS
LambertianBrdfAllocateWithAllocator(
    _Inout_ PBRDF_ALLOCATOR brdf_allocator,
    _In_ PCREFLECTOR reflector,
    _Out_ PCBRDF *brdf
    );

#if __cplusplus 
}
#endif // __cplusplus

#endif // _IRIS_PHYSX_TOOLKIT_LAMBERTIAN_BRDF_