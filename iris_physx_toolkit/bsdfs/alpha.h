/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    alpha.h

Abstract:

    Creates an alpha BSDF for blending.

--*/

#ifndef _IRIS_PHYSX_TOOLKIT_BSDFS_ALPHA_
#define _IRIS_PHYSX_TOOLKIT_BSDFS_ALPHA_

#include "iris_physx/iris_physx.h"

#if __cplusplus 
extern "C" {
#endif // __cplusplus

//
// Functions
//

ISTATUS
AlphaBsdfAllocateWithAllocator(
    _Inout_ PBSDF_ALLOCATOR bsdf_allocator,
    _In_opt_ PCBSDF base,
    _In_ float_t alpha,
    _Out_ PCBSDF *bsdf
    );

#if __cplusplus 
}
#endif // __cplusplus

#endif // _IRIS_PHYSX_TOOLKIT_BSDFS_ALPHA_