/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    lambertian.h

Abstract:

    Creates a Lambertian BSDF.

--*/

#ifndef _IRIS_PHYSX_TOOLKIT_BSDFS_LAMBERTIAN_
#define _IRIS_PHYSX_TOOLKIT_BSDFS_LAMBERTIAN_

#include "iris_physx/iris_physx.h"

#if __cplusplus 
extern "C" {
#endif // __cplusplus

//
// Functions
//

ISTATUS
LambertianBsdfAllocate(
    _In_opt_ PREFLECTOR reflector,
    _Out_ PBSDF *bsdf
    );

ISTATUS
LambertianBsdfAllocateWithAllocator(
    _Inout_ PBSDF_ALLOCATOR bsdf_allocator,
    _In_opt_ PCREFLECTOR reflector,
    _Out_ PCBSDF *bsdf
    );

#if __cplusplus 
}
#endif // __cplusplus

#endif // _IRIS_PHYSX_TOOLKIT_BSDFS_LAMBERTIAN_