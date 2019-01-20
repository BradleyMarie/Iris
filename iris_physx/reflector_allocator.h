/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    reflector_allocator.h

Abstract:

    Allows arbitrary reflections to be allocated cheaply at runtime avoiding
    the global memory allocator.

    Reflections live as long as the lifetime of the allocator, and are freed
    implicitly as soon as it goes out of scope.

    NOTE: Reflectors allocated using this allocator should not contain unique 
    references to dynamically allocated memory as no free logic is run when that
    reflector is freed and there is no opportunity to release any dynamically
    allocated memory back to they system.

--*/

#ifndef _IRIS_PHYSX_REFLECTOR_ALLOCATOR_
#define _IRIS_PHYSX_REFLECTOR_ALLOCATOR_

#include "iris_physx/reflector.h"

//
// Types
//

typedef struct _REFLECTOR_ALLOCATOR REFLECTOR_ALLOCATOR, *PREFLECTOR_ALLOCATOR;
typedef const REFLECTOR_ALLOCATOR *PCREFLECTOR_ALLOCATOR;

//
// Functions
//

ISTATUS
ReflectorAllocatorAllocate(
    _Inout_ PREFLECTOR_ALLOCATOR allocator,
    _In_ PCREFLECTOR_VTABLE vtable,
    _In_reads_bytes_opt_(data_size) const void *data,
    _In_ size_t data_size,
    _In_ size_t data_alignment,
    _Out_ PCREFLECTOR *reflector
    );

#endif // _IRIS_PHYSX_REFLECTOR_ALLOCATOR_