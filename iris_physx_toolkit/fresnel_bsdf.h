/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    lambertian_bsdf.h

Abstract:

    Creates a Fresnel BSDF.

--*/

#ifndef _IRIS_PHYSX_TOOLKIT_LAMBERTIAN_BSDF_
#define _IRIS_PHYSX_TOOLKIT_LAMBERTIAN_BSDF_

#include "iris_physx/iris_physx.h"

#if __cplusplus 
extern "C" {
#endif // __cplusplus

//
// Functions
//

ISTATUS
SpecularDielectricBsdfAllocate(
    _In_ PREFLECTOR reflected,
    _In_ PREFLECTOR transmitted,
    _In_ float_t refractive_index_incident,
    _In_ float_t refractive_index_transmitted,
    _Out_ PBSDF *bsdf
    );

ISTATUS
SpecularDielectricBsdfAllocateWithAllocator(
    _Inout_ PBSDF_ALLOCATOR bsdf_allocator,
    _In_ PCREFLECTOR reflected,
    _In_ PCREFLECTOR transmitted,
    _In_ float_t refractive_index_incident,
    _In_ float_t refractive_index_transmitted,
    _Out_ PCBSDF *bsdf
    );

#if __cplusplus 
}
#endif // __cplusplus

#endif // _IRIS_PHYSX_TOOLKIT_LAMBERTIAN_BSDF_