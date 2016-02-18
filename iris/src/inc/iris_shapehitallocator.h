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

typedef struct _SHARED_HIT_DATA {
    PCMATRIX_REFERENCE ModelToWorld;
    BOOL Premultiplied;
    RAY ModelRay;
} SHARED_HIT_DATA, *PSHARED_HIT_DATA;

typedef CONST SHARED_HIT_DATA *PCSHARED_HIT_DATA;

typedef struct _SHAPE_HIT_ALLOCATOR {
    DYNAMIC_MEMORY_ALLOCATOR AdditionalDataAllocator;
    STATIC_MEMORY_ALLOCATOR ShapeHitAllocator;
    PCSHAPE_REFERENCE CurrentShapeReference;
} SHAPE_HIT_ALLOCATOR, *PSHAPE_HIT_ALLOCATOR;

typedef struct _INTERNAL_SHAPE_HIT {
    SHAPE_HIT ShapeHit;
    PCSHARED_HIT_DATA SharedHitData;
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
    PDYNAMIC_MEMORY_ALLOCATOR AdditionalDataAllocator;
    PSTATIC_MEMORY_ALLOCATOR ShapeHitAllocator;
    ISTATUS Status;

    ASSERT(ShapeHitCollection != NULL);

    ShapeHitAllocator = &ShapeHitCollection->ShapeHitAllocator;

    Status = StaticMemoryAllocatorInitialize(ShapeHitAllocator,
                                             sizeof(SHAPE_HIT_ALLOCATOR_ALLOCATION));

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    AdditionalDataAllocator = &ShapeHitCollection->AdditionalDataAllocator;

    DynamicMemoryAllocatorInitialize(AdditionalDataAllocator);  

    return ISTATUS_SUCCESS;
}

SFORCEINLINE
VOID
ShapeHitAllocatorDestroy(
    _Inout_ PSHAPE_HIT_ALLOCATOR Allocator
    )
{
    PDYNAMIC_MEMORY_ALLOCATOR AdditionalDataAllocator;
    PSTATIC_MEMORY_ALLOCATOR ShapeHitAllocator;

    ASSERT(Allocator != NULL);

    AdditionalDataAllocator = &Allocator->AdditionalDataAllocator;
    ShapeHitAllocator = &Allocator->ShapeHitAllocator;

    DynamicMemoryAllocatorDestroy(AdditionalDataAllocator);
    StaticMemoryAllocatorDestroy(ShapeHitAllocator);
}

SFORCEINLINE
VOID
ShapeHitAllocatorFreeAll(
    _Inout_ PSHAPE_HIT_ALLOCATOR Allocator
    )
{
    PDYNAMIC_MEMORY_ALLOCATOR AdditionalDataAllocator;
    PSTATIC_MEMORY_ALLOCATOR ShapeHitAllocator;

    ASSERT(Allocator != NULL);

    AdditionalDataAllocator = &Allocator->AdditionalDataAllocator;
    ShapeHitAllocator = &Allocator->ShapeHitAllocator;

    DynamicMemoryAllocatorFreeAll(AdditionalDataAllocator);
    StaticMemoryAllocatorFreeAll(ShapeHitAllocator);
}

SFORCEINLINE
VOID
ShapeHitAllocatorSetCurrentShapeReference(
    _Inout_ PSHAPE_HIT_ALLOCATOR Allocator,
    _In_ PCSHAPE_REFERENCE ShapeReference
    )
{
    ASSERT(Allocator != NULL);
    ASSERT(ShapeReference != NULL);

    Allocator->CurrentShapeReference = ShapeReference;
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