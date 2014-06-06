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

typedef struct _INTERNAL_SHAPE_HIT {
    SHAPE_HIT ShapeHit;
    PCSHARED_GEOMETRY_HIT SharedGeometryHit;
    POINT3 ModelHitPoint;
    BOOL ModelHitPointValid;
} INTERNAL_SHAPE_HIT, *PINTERNAL_SHAPE_HIT;

typedef CONST INTERNAL_SHAPE_HIT *PCINTERNAL_SHAPE_HIT;

typedef struct _SHAPE_HIT_ALLOCATOR_ALLOCATION {
    SHAPE_HIT_LIST ShapeHitList;
    INTERNAL_SHAPE_HIT InternalShapeHit;
} SHAPE_HIT_ALLOCATOR_ALLOCATION, *PSHAPE_HIT_ALLOCATOR_ALLOCATION;

typedef CONST SHAPE_HIT_ALLOCATOR_ALLOCATION *PCSHAPE_HIT_ALLOCATOR_ALLOCATION;

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
                                                 sizeof(SHAPE_HIT_ALLOCATOR_ALLOCATION));

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

SFORCEINLINE
COMPARISON_RESULT
InternalShapeHitCompare(
    _In_ PCINTERNAL_SHAPE_HIT Hit0,
    _In_ PCINTERNAL_SHAPE_HIT Hit1
    )
{
    ASSERT(Hit0 != NULL);
    ASSERT(Hit1 != NULL);

    return (Hit0->ShapeHit.Distance <= Hit1->ShapeHit.Distance) ? -1 : 1;
}

#endif // _IRIS_SHAPE_HIT_ALLOCATOR_INTERNAL_