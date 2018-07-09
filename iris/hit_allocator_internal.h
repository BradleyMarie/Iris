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
    const RAY *model_ray;
    const void *data;
    float_t minimum_distance;
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
    allocator->model_ray = NULL;
    allocator->data = NULL;
}

static
inline 
void
HitAllocatorSetRay(
    _Inout_ struct _HIT_ALLOCATOR *allocator,
    _In_ PCRAY model_ray
    )
{
    assert(allocator != NULL);
    assert(model_ray != NULL);
    assert(RayValidate(*model_ray));

    allocator->model_ray = model_ray;
}

_Ret_
static
inline 
PCRAY
HitAllocatorGetRay(
    _In_ const struct _HIT_ALLOCATOR *allocator
    )
{
    assert(allocator != NULL);

    return allocator->model_ray;
}

static
inline 
void
HitAllocatorSetData(
    _Inout_ struct _HIT_ALLOCATOR *allocator,
    _In_ const void *data
    )
{
    assert(allocator != NULL);

    allocator->data = data;
}

_Ret_
static
inline 
const void *
HitAllocatorGetData(
    _In_ const struct _HIT_ALLOCATOR *allocator
    )
{
    assert(allocator != NULL);

    return allocator->data;
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

#endif // _IRIS_HIT_ALLOCATOR_INTERNAL_