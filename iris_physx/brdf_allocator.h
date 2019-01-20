/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    brdf_allocator.h

Abstract:

    Allows arbitrary BRDFs to be allocated cheaply at runtime avoiding
    the global memory allocator.

    BRDFs live as long as the lifetime of the allocator, and are freed
    implicitly as soon as it goes out of scope.

    NOTE: BRDFs allocated using this allocator should not contain unique 
    references to dynamically allocated memory as no free logic is run when that
    reflector is freed and there is no opportunity to release any dynamically
    allocated memory back to they system.

--*/

#ifndef _IRIS_PHYSX_BRDF_ALLOCATOR_
#define _IRIS_PHYSX_BRDF_ALLOCATOR_

#include "iris_physx/brdf.h"

//
// Types
//

typedef struct _BRDF_ALLOCATOR BRDF_ALLOCATOR, *PBRDF_ALLOCATOR;
typedef const BRDF_ALLOCATOR *PCBRDF_ALLOCATOR;

//
// Functions
//

ISTATUS
BrdfAllocatorAllocate(
    _Inout_ PBRDF_ALLOCATOR allocator,
    _In_ PCBRDF_VTABLE vtable,
    _In_reads_bytes_opt_(data_size) const void *data,
    _In_ size_t data_size,
    _In_ size_t data_alignment,
    _Out_ PCBRDF *brdf
    );

#endif // _IRIS_PHYSX_BRDF_ALLOCATOR_