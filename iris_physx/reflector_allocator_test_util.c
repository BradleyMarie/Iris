/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    reflector_allocator_test_util.c

Abstract:

    Adapter for allocating reflector allocator from tests, since C++ doesn't 
    support loading stdatomic.h

--*/

#include "iris_physx/reflector_allocator_internal.h"
#include "iris_physx/reflector_allocator_test_util.h"

_Ret_maybenull_
PREFLECTOR_ALLOCATOR
ReflectorAllocatorCreate(
    void
    )
{
    PREFLECTOR_ALLOCATOR allocator = 
        (PREFLECTOR_ALLOCATOR) malloc(sizeof(REFLECTOR_ALLOCATOR));
    if (allocator == NULL)
    {
        return NULL;
    }

    ReflectorAllocatorInitialize(allocator);

    return allocator;
}

void
ReflectorAllocatorFree(
    _In_opt_ _Post_invalid_ PREFLECTOR_ALLOCATOR allocator
    )
{
    if (allocator == NULL)
    {
        return;
    }

    ReflectorAllocatorDestroy(allocator);
    free(allocator);
}