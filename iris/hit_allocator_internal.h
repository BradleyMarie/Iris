/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    hit_allocator_internal.h

Abstract:

    Allocator for hit objects.

--*/

#ifndef _IRIS_HIT_ALLOCATOR_INTERNAL_
#define _IRIS_HIT_ALLOCATOR_INTERNAL_

#include "common/dynamic_allocator.h"

//
// Types
//

typedef struct _HIT_ALLOCATOR {
    DYNAMIC_MEMORY_ALLOCATOR allocator;
} HIT_ALLOCATOR, *PHIT_ALLOCATOR;

//
// Functions
//

static
inline
void
HitAllocatorInitialize(
    _Out_ struct _HIT_ALLOCATOR *allocator
    )
{
    assert(allocator != NULL);

    DynamicMemoryAllocatorInitialize(&allocator->allocator);
}

static
inline
void
HitAllocatorDestroy(
    _Inout_ struct _HIT_ALLOCATOR *allocator
    )
{
    assert(allocator != NULL);

    DynamicMemoryAllocatorDestroy(&allocator->allocator);
}

static
inline
void
HitAllocatorFreeAll(
    _Inout_ struct _HIT_ALLOCATOR *allocator
    )
{
    assert(allocator != NULL);

    DynamicMemoryAllocatorFreeAll(&allocator->allocator);
}

#endif // _IRIS_HIT_ALLOCATOR_INTERNAL_