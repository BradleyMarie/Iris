/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    bsdf_allocator_internal.h

Abstract:

    Internal headers for BSDF allocator.

--*/

#ifndef _IRIS_PHYSX_BSDF_ALLOCATOR_INTERNAL_
#define _IRIS_PHYSX_BSDF_ALLOCATOR_INTERNAL_

#include "common/dynamic_allocator.h"
#include "iris_physx/bsdf_internal.h"

//
// Types
//

struct _BSDF_ALLOCATOR {
    DYNAMIC_MEMORY_ALLOCATOR allocator;
};

//
// Functions
//

static
inline
void
BsdfAllocatorInitialize(
    _Out_ struct _BSDF_ALLOCATOR *allocator
    )
{
    assert(allocator != NULL);

    DynamicMemoryAllocatorInitialize(&allocator->allocator);
}

static
inline
void
BsdfAllocatorClear(
    _Inout_ struct _BSDF_ALLOCATOR *allocator
    )
{
    assert(allocator != NULL);

    DynamicMemoryAllocatorFreeAll(&allocator->allocator);
}

static
inline
void
BsdfAllocatorDestroy(
    _Inout_ struct _BSDF_ALLOCATOR *allocator
    )
{
    assert(allocator != NULL);

    DynamicMemoryAllocatorDestroy(&allocator->allocator);
}

#endif // _IRIS_PHYSX_BSDF_ALLOCATOR_INTERNAL_