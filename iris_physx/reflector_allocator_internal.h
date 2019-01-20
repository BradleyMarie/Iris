/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    reflector_allocator_internal.h

Abstract:

    Internal headers for reflector allocator.

--*/

#ifndef _IRIS_PHYSX_REFLECTOR_ALLOCATOR_INTERNAL_
#define _IRIS_PHYSX_REFLECTOR_ALLOCATOR_INTERNAL_

#include "common/dynamic_allocator.h"
#include "iris_physx/reflector_internal.h"

//
// Types
//

struct _REFLECTOR_ALLOCATOR {
    DYNAMIC_MEMORY_ALLOCATOR allocator;
};

//
// Functions
//

static
inline
void
ReflectorAllocatorInitialize(
    _Out_ struct _REFLECTOR_ALLOCATOR *allocator
    )
{
    assert(allocator != NULL);

    DynamicMemoryAllocatorInitialize(&allocator->allocator);
}

static
inline
void
ReflectorAllocatorClear(
    _Inout_ struct _REFLECTOR_ALLOCATOR *allocator
    )
{
    assert(allocator != NULL);

    DynamicMemoryAllocatorFreeAll(&allocator->allocator);
}

static
inline
void
ReflectorAllocatorDestroy(
    _Inout_ struct _REFLECTOR_ALLOCATOR *allocator
    )
{
    assert(allocator != NULL);

    DynamicMemoryAllocatorDestroy(&allocator->allocator);
}

#endif // _IRIS_PHYSX_REFLECTOR_ALLOCATOR_INTERNAL_