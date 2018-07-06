/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    brdf_allocator_internal.h

Abstract:

    Internal headers for BRDF allocator.

--*/

#ifndef _IRIS_PHYSX_BRDF_ALLOCATOR_INTERNAL_
#define _IRIS_PHYSX_BRDF_ALLOCATOR_INTERNAL_

#include "common/dynamic_allocator.h"
#include "iris_physx/brdf_internal.h"

//
// Types
//

struct _BRDF_ALLOCATOR {
    DYNAMIC_MEMORY_ALLOCATOR allocator;
};

//
// Functions
//

static
inline
void
BrdfAllocatorInitialize(
    _Out_ struct _BRDF_ALLOCATOR *allocator
    )
{
    assert(allocator != NULL);

    DynamicMemoryAllocatorInitialize(&allocator->allocator);
}

static
inline
void
BrdfAllocatorClear(
    _Inout_ struct _BRDF_ALLOCATOR *allocator
    )
{
    assert(allocator != NULL);

    DynamicMemoryAllocatorFreeAll(&allocator->allocator);
}

static
inline
void
BrdfAllocatorDestroy(
    _Inout_ struct _BRDF_ALLOCATOR *allocator
    )
{
    assert(allocator != NULL);

    DynamicMemoryAllocatorDestroy(&allocator->allocator);
}

#endif // _IRIS_PHYSX_BRDF_ALLOCATOR_INTERNAL_