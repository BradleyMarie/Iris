/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    shared_hit_context_allocator.h

Abstract:

    Allocator for shared hit context objects.

--*/

#ifndef _IRIS_SHARED_HIT_CONTEXT_ALLOCATOR_
#define _IRIS_SHARED_HIT_CONTEXT_ALLOCATOR_

#include "common/static_allocator.h"
#include "iris/shared_hit_context.h"

//
// Types
//

typedef struct _SHARED_HIT_CONTEXT_ALLOCATOR {
    STATIC_MEMORY_ALLOCATOR allocator;
} SHARED_HIT_CONTEXT_ALLOCATOR, *PSHARED_HIT_CONTEXT_ALLOCATOR;

//
// Functions
//

static
inline
bool
SharedHitContextAllocatorInitialize(
    _Out_ PSHARED_HIT_CONTEXT_ALLOCATOR allocator
    )
{
    assert(allocator != NULL);

    bool success = StaticMemoryAllocatorInitialize(&allocator->allocator,
                                                   sizeof(SHARED_HIT_CONTEXT));

    return success;
}

static
inline
bool
SharedHitContextAllocatorAllocatePremultiplied(
    _Inout_ PSHARED_HIT_CONTEXT_ALLOCATOR allocator,
    _In_ PCMATRIX model_to_world,
    _Out_ PSHARED_HIT_CONTEXT *context
    )
{
    assert(allocator != NULL);
    assert(model_to_world != NULL);
    assert(context != NULL);

    bool success = StaticMemoryAllocatorAllocate(&allocator->allocator,
                                                 (void **)context);

    if (!success)
    {
        return false;
    }

    (*context)->model_to_world = model_to_world;
    (*context)->premultiplied = true;

    return true;
}

static
inline
bool
SharedHitContextAllocatorAllocateTransformed(
    _Inout_ PSHARED_HIT_CONTEXT_ALLOCATOR allocator,
    _In_ PCMATRIX model_to_world,
    _In_ RAY model_ray,
    _Out_ PSHARED_HIT_CONTEXT *context
    )
{
    assert(allocator != NULL);
    assert(model_to_world != NULL);
    assert(RayValidate(model_ray));
    assert(context != NULL);

    bool success = StaticMemoryAllocatorAllocate(&allocator->allocator,
                                                 (void **)context);

    if (!success)
    {
        return false;
    }

    (*context)->model_to_world = model_to_world;
    (*context)->premultiplied = false;
    (*context)->model_ray = model_ray;

    return true;
}

static
inline
void
SharedHitContextAllocatorFreeAll(
    _Inout_ PSHARED_HIT_CONTEXT_ALLOCATOR allocator
    )
{
    assert(allocator != NULL);

    StaticMemoryAllocatorFreeAll(&allocator->allocator);
}

static
inline
void
SharedHitContextAllocatorDestroy(
    _Inout_ PSHARED_HIT_CONTEXT_ALLOCATOR allocator
    )
{
    assert(allocator != NULL);

    StaticMemoryAllocatorDestroy(&allocator->allocator);
}

#endif // _IRIS_SHARED_HIT_CONTEXT_ALLOCATOR_