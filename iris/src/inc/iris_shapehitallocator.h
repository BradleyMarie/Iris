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
    IRIS_STATIC_MEMORY_ALLOCATOR ShapeHitAllocator;
} SHAPE_HIT_ALLOCATOR, *PSHAPE_HIT_ALLOCATOR;

typedef struct _FULL_SHAPE_HIT {
    SHAPE_HIT_LIST ShapeHitList;
    SHAPE_HIT ShapeHit;
} FULL_SHAPE_HIT, *PFULL_SHAPE_HIT;

typedef CONST FULL_SHAPE_HIT *PCFULL_SHAPE_HIT;

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
    PIRIS_STATIC_MEMORY_ALLOCATOR ShapeHitAllocator;
    ISTATUS Status;

    ASSERT(ShapeHitCollection != NULL);

    ShapeHitAllocator = &ShapeHitCollection->ShapeHitAllocator;

    Status = IrisStaticMemoryAllocatorInitialize(ShapeHitAllocator,
                                                 sizeof(FULL_SHAPE_HIT));

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
    _Inout_ PSHAPE_HIT_ALLOCATOR Allocator
    )
{
    PIRIS_DYNAMIC_MEMORY_ALLOCATOR AdditionalDataAllocator;
    PIRIS_STATIC_MEMORY_ALLOCATOR ShapeHitAllocator;

    ASSERT(Allocator != NULL);

    AdditionalDataAllocator = &Allocator->AdditionalDataAllocator;
    ShapeHitAllocator = &Allocator->ShapeHitAllocator;

    IrisDynamicMemoryAllocatorDestroy(AdditionalDataAllocator);
    IrisStaticMemoryAllocatorDestroy(ShapeHitAllocator);
}

SFORCEINLINE
VOID
ShapeHitAllocatorFreeAll(
    _Inout_ PSHAPE_HIT_ALLOCATOR Allocator
    )
{
    PIRIS_DYNAMIC_MEMORY_ALLOCATOR AdditionalDataAllocator;
    PIRIS_STATIC_MEMORY_ALLOCATOR ShapeHitAllocator;

    ASSERT(Allocator != NULL);

    AdditionalDataAllocator = &Allocator->AdditionalDataAllocator;
    ShapeHitAllocator = &Allocator->ShapeHitAllocator;

    IrisDynamicMemoryAllocatorFreeAll(AdditionalDataAllocator);
    IrisStaticMemoryAllocatorFreeAll(ShapeHitAllocator);
}

#endif // _IRIS_SHAPE_HIT_ALLOCATOR_INTERNAL_