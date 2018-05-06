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

_Check_return_
_Success_(return != 0)
static
inline
bool
HitAllocatorInitialize(
    _Out_ PHIT_ALLOCATOR allocator
    )
{
    ASSERT(allocator != NULL);

    return DynamicMemoryAllocatorInitialize(&allocator->allocator);
}

static
inline
void
HitAllocatorDestroy(
    _Inout_ PHIT_ALLOCATOR allocator
    )
{
    ASSERT(allocator != NULL);

    DynamicMemoryAllocatorDestroy(&allocator->allocator);
}

static
inline
void
HitAllocatorFreeAll(
    _Inout_ PHIT_ALLOCATOR allocator
    )
{
    ASSERT(allocator != NULL);

    DynamicMemoryAllocatorFreeAll(&allocator->allocator);
}

#endif // _IRIS_HIT_ALLOCATOR_INTERNAL_