/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    mirror_bsdf.h

Abstract:

    Creates a mirror BSDF.

--*/

#ifndef _IRIS_PHYSX_TOOLKIT_MIRROR_BSDF_
#define _IRIS_PHYSX_TOOLKIT_MIRROR_BSDF_

#include "iris_physx/iris_physx.h"

#if __cplusplus 
extern "C" {
#endif // __cplusplus

//
// Functions
//

ISTATUS
MirrorReflectorAllocate(
    _In_ PREFLECTOR reflector,
    _Out_ PBSDF *bsdf
    );

ISTATUS
MirrorReflectorAllocateWithAllocator(
    _Inout_ PBSDF_ALLOCATOR bsdf_allocator,
    _In_ PCREFLECTOR reflector,
    _Out_ PCBSDF *bsdf
    );

#if __cplusplus 
}
#endif // __cplusplus

#endif // _IRIS_PHYSX_TOOLKIT_MIRROR_BSDF_