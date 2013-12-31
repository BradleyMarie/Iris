/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    iris_shapehitallocator.h

Abstract:

    This module implements the SHAPE_HIT_ALLOCATOR internal functions.

--*/

#ifndef _IRIS_SHAPE_HIT_ALLOCATOR_INTERNAL_
#define _IRIS_SHAPE_HIT_ALLOCATOR_INTERNAL_

#include <irisp.h>

//
// Types
//

typedef struct _SHAPE_HIT_ALLOCATOR {
    IRIS_DYNAMIC_MEMORY_ALLOCATOR AdditionalDataAllocator;
    IRIS_STATIC_MEMORY_ALLOCATOR GeometryHitAllocator;
} SHAPE_HIT_ALLOCATOR, *PSHAPE_HIT_ALLOCATOR;

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
ShapeHitAllocatorInitialize(
    _Out_ PSHAPE_HIT_ALLOCATOR ShapeHitCollection
    )
{
    PIRIS_DYNAMIC_MEMORY_ALLOCATOR AdditionalDataAllocator;
    PIRIS_STATIC_MEMORY_ALLOCATOR GeometryHitAllocator;
    ISTATUS Status;

    ASSERT(ShapeHitCollection != NULL);

    GeometryHitAllocator = &ShapeHitCollection->GeometryHitAllocator;

    Status = IrisStaticMemoryAllocatorInitialize(GeometryHitAllocator,
                                                 sizeof(GEOMETRY_HIT));

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    AdditionalDataAllocator = &ShapeHitCollection->AdditionalDataAllocator;

    IrisDynamicMemoryAllocatorInitialize(AdditionalDataAllocator);  

    return ISTATUS_SUCCESS;
}

SFORCEINLINE
VOID
ShapeHitAllocatorDestroy(
    _Inout_ PSHAPE_HIT_ALLOCATOR ShapeHitAllocator
    )
{
    PIRIS_DYNAMIC_MEMORY_ALLOCATOR AdditionalDataAllocator;
    PIRIS_STATIC_MEMORY_ALLOCATOR GeometryHitAllocator;

    ASSERT(ShapeHitAllocator != NULL);

    AdditionalDataAllocator = &ShapeHitAllocator->AdditionalDataAllocator;
    GeometryHitAllocator = &ShapeHitAllocator->GeometryHitAllocator;

    IrisDynamicMemoryAllocatorDestroy(AdditionalDataAllocator);
    IrisStaticMemoryAllocatorDestroy(GeometryHitAllocator);
}

SFORCEINLINE
VOID
ShapeHitAllocatorFreeAll(
    _Inout_ PSHAPE_HIT_ALLOCATOR ShapeHitAllocator
    )
{
    PIRIS_DYNAMIC_MEMORY_ALLOCATOR AdditionalDataAllocator;
    PIRIS_STATIC_MEMORY_ALLOCATOR GeometryHitAllocator;

    ASSERT(ShapeHitAllocator != NULL);

    AdditionalDataAllocator = &ShapeHitAllocator->AdditionalDataAllocator;
    GeometryHitAllocator = &ShapeHitAllocator->GeometryHitAllocator;

    IrisDynamicMemoryAllocatorFreeAll(AdditionalDataAllocator);
    IrisStaticMemoryAllocatorFreeAll(GeometryHitAllocator);
}

#endif // _IRIS_SHAPE_HIT_ALLOCATOR_INTERNAL_