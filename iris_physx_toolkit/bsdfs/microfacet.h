/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    microfacet.h

Abstract:

    Creates the microfacet BSDF.

--*/

#ifndef _IRIS_PHYSX_TOOLKIT_BSDFS_MICROFACET_
#define _IRIS_PHYSX_TOOLKIT_BSDFS_MICROFACET_

#include "iris_physx_toolkit/bsdfs/fresnels/fresnel.h"
#include "iris_physx_toolkit/bsdfs/microfacet_distributions/microfacet_distribution.h"

#if __cplusplus 
extern "C" {
#endif // __cplusplus

//
// Functions
//

ISTATUS
MicrofacetReflectionBsdfAllocateWithAllocator(
    _Inout_ PBSDF_ALLOCATOR bsdf_allocator,
    _In_opt_ PCREFLECTOR reflector,
    _In_ PCMICROFACET_DISTRIBUTION microfacet_distribution,
    _In_ PCFRESNEL fresnel_function,
    _Out_ PCBSDF *bsdf
    );

#if __cplusplus 
}
#endif // __cplusplus

#endif // _IRIS_PHYSX_TOOLKIT_BSDFS_MICROFACET_