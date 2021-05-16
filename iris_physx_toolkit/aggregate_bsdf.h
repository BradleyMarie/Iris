/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    aggregate_bsdf.h

Abstract:

    Creates a aggregate BSDF.

--*/

#ifndef _IRIS_PHYSX_TOOLKIT_AGGREGATE_BSDF_
#define _IRIS_PHYSX_TOOLKIT_AGGREGATE_BSDF_

#include "iris_physx/iris_physx.h"

#if __cplusplus 
extern "C" {
#endif // __cplusplus

//
// Functions
//

ISTATUS
AggregateBsdfAllocate(
    _In_reads_(num_bsdfs) PBSDF *bsdfs,
    _In_ size_t num_bsdfs,
    _Out_ PBSDF *bsdf
    );

ISTATUS
AggregateBsdfAllocateWithAllocator(
    _Inout_ PBSDF_ALLOCATOR bsdf_allocator,
    _In_reads_(num_bsdfs) PCBSDF *bsdfs,
    _In_ size_t num_bsdfs,
    _Out_ PCBSDF *bsdf
    );

#if __cplusplus 
}
#endif // __cplusplus

#endif // _IRIS_PHYSX_TOOLKIT_AGGREGATE_BSDF_