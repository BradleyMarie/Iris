/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    microfacet_bsdf.h

Abstract:

    Creates the microfacet BSDFs.

--*/

#ifndef _IRIS_PHYSX_TOOLKIT_MICROFACET_BSDF_
#define _IRIS_PHYSX_TOOLKIT_MICROFACET_BSDF_

#include "iris_physx/iris_physx.h"

#if __cplusplus 
extern "C" {
#endif // __cplusplus

//
// Functions
//

ISTATUS
TrowbridgeReitzDielectricReflectionBsdfAllocate(
    _In_opt_ PREFLECTOR reflector,
    _In_ float_t alpha_x,
    _In_ float_t alpha_y,
    _In_ float_t eta_i,
    _In_ float_t eta_t,
    _Out_ PBSDF *bsdf
    );

ISTATUS
TrowbridgeReitzDielectricReflectionBsdfAllocateWithAllocator(
    _Inout_ PBSDF_ALLOCATOR bsdf_allocator,
    _In_opt_ PCREFLECTOR reflector,
    _In_ float_t alpha_x,
    _In_ float_t alpha_y,
    _In_ float_t eta_i,
    _In_ float_t eta_t,
    _Out_ PCBSDF *bsdf
    );

#if __cplusplus 
}
#endif // __cplusplus

#endif // _IRIS_PHYSX_TOOLKIT_MICROFACET_BSDF_