/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    oren_nayar.h

Abstract:

    Creates a Oren-Nayar BSDF.

--*/

#ifndef _IRIS_PHYSX_TOOLKIT_BSDFS_OREN_NAYAR_
#define _IRIS_PHYSX_TOOLKIT_BSDFS_OREN_NAYAR_

#include "iris_physx/iris_physx.h"

#if __cplusplus 
extern "C" {
#endif // __cplusplus

//
// Functions
//

ISTATUS
OrenNayarBsdfAllocate(
    _In_opt_ PREFLECTOR reflector,
    _In_ float_t sigma,
    _Out_ PBSDF *bsdf
    );

ISTATUS
OrenNayarBsdfAllocateWithAllocator(
    _Inout_ PBSDF_ALLOCATOR bsdf_allocator,
    _In_opt_ PCREFLECTOR reflector,
    _In_ float_t sigma,
    _Out_ PCBSDF *bsdf
    );

#if __cplusplus 
}
#endif // __cplusplus

#endif // _IRIS_PHYSX_TOOLKIT_BSDFS_OREN_NAYAR_