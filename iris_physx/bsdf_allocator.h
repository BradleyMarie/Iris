/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    bsdf_allocator.h

Abstract:

    Allows arbitrary BSDFs to be allocated cheaply at runtime avoiding
    the global memory allocator.

    BSDFs live as long as the lifetime of the allocator, and are freed
    implicitly as soon as it goes out of scope.

    NOTE: BSDFs allocated using this allocator should not contain unique 
    references to dynamically allocated memory as no free logic is run when that
    reflector is freed and there is no opportunity to release any dynamically
    allocated memory back to they system.

--*/

#ifndef _IRIS_PHYSX_BSDF_ALLOCATOR_
#define _IRIS_PHYSX_BSDF_ALLOCATOR_

#include "iris_physx/bsdf.h"

//
// Types
//

typedef struct _BSDF_ALLOCATOR BSDF_ALLOCATOR, *PBSDF_ALLOCATOR;
typedef const BSDF_ALLOCATOR *PCBSDF_ALLOCATOR;

//
// Functions
//

ISTATUS
BsdfAllocatorAllocate(
    _Inout_ PBSDF_ALLOCATOR allocator,
    _In_ PCBSDF_VTABLE vtable,
    _In_reads_bytes_opt_(data_size) const void *data,
    _In_ size_t data_size,
    _In_ size_t data_alignment,
    _Out_ PCBSDF *bsdf
    );

#endif // _IRIS_PHYSX_BSDF_ALLOCATOR_