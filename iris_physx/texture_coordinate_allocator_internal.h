/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    spectrum_internal.h

Abstract:

    The internal routines for texture coordinate allocator.

--*/

#ifndef _IRIS_PHYSX_TEXTURE_COORDINATE_ALLOCATOR_INTERNAL_
#define _IRIS_PHYSX_TEXTURE_COORDINATE_ALLOCATOR_INTERNAL_

#include "common/dynamic_allocator.h"

//
// Types
//

struct _TEXTURE_COORDINATE_ALLOCATOR {
    DYNAMIC_MEMORY_ALLOCATOR allocator;
};

//
// Functions
//

static
inline
void
TextureCoordinateAllocatorInitialize(
    _Out_ struct _TEXTURE_COORDINATE_ALLOCATOR *allocator
    )
{
    assert(allocator != NULL);

    DynamicMemoryAllocatorInitialize(&allocator->allocator);
}

static
inline
void
TextureCoordinateAllocatorClear(
    _Out_ struct _TEXTURE_COORDINATE_ALLOCATOR *allocator
    )
{
    assert(allocator != NULL);

    DynamicMemoryAllocatorFreeAll(&allocator->allocator);
}

static
inline
void
TextureCoordinateAllocatorDestroy(
    _Inout_ struct _TEXTURE_COORDINATE_ALLOCATOR *allocator
    )
{
    assert(allocator != NULL);

    DynamicMemoryAllocatorDestroy(&allocator->allocator);
}

#endif // _IRIS_PHYSX_TEXTURE_COORDINATE_ALLOCATOR_INTERNAL_