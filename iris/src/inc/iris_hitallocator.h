/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    iris_hitallocator.h

Abstract:

    This module implements the HIT_ALLOCATOR internal functions.

--*/

#ifndef _IRIS_HIT_ALLOCATOR_INTERNAL_
#define _IRIS_HIT_ALLOCATOR_INTERNAL_

#include <irisp.h>

//
// Types
//

typedef struct _SHARED_HIT_DATA {
    PCMATRIX ModelToWorld;
    BOOL Premultiplied;
    RAY ModelRay;
} SHARED_HIT_DATA, *PSHARED_HIT_DATA;

typedef CONST SHARED_HIT_DATA *PCSHARED_HIT_DATA;

typedef struct _HIT_ALLOCATOR {
    DYNAMIC_MEMORY_ALLOCATOR AdditionalDataAllocator;
    STATIC_MEMORY_ALLOCATOR HitAllocator;
    PCSHAPE CurrentShape;
} HIT_ALLOCATOR, *PHIT_ALLOCATOR;

typedef struct _INTERNAL_HIT {
    HIT Hit;
    PCSHARED_HIT_DATA SharedHitData;
    POINT3 ModelHitPoint;
    BOOL ModelHitPointValid;
} INTERNAL_HIT, *PINTERNAL_HIT;

typedef CONST INTERNAL_HIT *PCINTERNAL_HIT;

typedef struct _HIT_ALLOCATOR_ALLOCATION {
    HIT_LIST HitList;
    INTERNAL_HIT InternalHit;
} HIT_ALLOCATOR_ALLOCATION, *PHIT_ALLOCATOR_ALLOCATION;

typedef CONST HIT_ALLOCATOR_ALLOCATION *PCHIT_ALLOCATOR_ALLOCATION;

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
HitAllocatorInitialize(
    _Out_ PHIT_ALLOCATOR HitCollection
    )
{
    PDYNAMIC_MEMORY_ALLOCATOR AdditionalDataAllocator;
    PSTATIC_MEMORY_ALLOCATOR HitAllocator;
    ISTATUS Status;

    ASSERT(HitCollection != NULL);

    HitAllocator = &HitCollection->HitAllocator;

    Status = StaticMemoryAllocatorInitialize(HitAllocator,
                                             sizeof(HIT_ALLOCATOR_ALLOCATION));

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    AdditionalDataAllocator = &HitCollection->AdditionalDataAllocator;

    DynamicMemoryAllocatorInitialize(AdditionalDataAllocator);  

    return ISTATUS_SUCCESS;
}

SFORCEINLINE
VOID
HitAllocatorDestroy(
    _Inout_ PHIT_ALLOCATOR Allocator
    )
{
    PDYNAMIC_MEMORY_ALLOCATOR AdditionalDataAllocator;
    PSTATIC_MEMORY_ALLOCATOR HitAllocator;

    ASSERT(Allocator != NULL);

    AdditionalDataAllocator = &Allocator->AdditionalDataAllocator;
    HitAllocator = &Allocator->HitAllocator;

    DynamicMemoryAllocatorDestroy(AdditionalDataAllocator);
    StaticMemoryAllocatorDestroy(HitAllocator);
}

SFORCEINLINE
VOID
HitAllocatorFreeAll(
    _Inout_ PHIT_ALLOCATOR Allocator
    )
{
    PDYNAMIC_MEMORY_ALLOCATOR AdditionalDataAllocator;
    PSTATIC_MEMORY_ALLOCATOR HitAllocator;

    ASSERT(Allocator != NULL);

    AdditionalDataAllocator = &Allocator->AdditionalDataAllocator;
    HitAllocator = &Allocator->HitAllocator;

    DynamicMemoryAllocatorFreeAll(AdditionalDataAllocator);
    StaticMemoryAllocatorFreeAll(HitAllocator);
}

SFORCEINLINE
VOID
HitAllocatorSetCurrentShape(
    _Inout_ PHIT_ALLOCATOR Allocator,
    _In_ PCSHAPE Shape
    )
{
    ASSERT(Allocator != NULL);
    ASSERT(Shape != NULL);

    Allocator->CurrentShape = Shape;
}

SFORCEINLINE
PCSHAPE
HitAllocatorGetCurrentShape(
    _Inout_ PHIT_ALLOCATOR Allocator
    )
{
    PCSHAPE Result;
    
    ASSERT(Allocator != NULL);

    Result = Allocator->CurrentShape;
    
    return Result;
}

SFORCEINLINE
COMPARISON_RESULT
InternalHitCompare(
    _In_ PCINTERNAL_HIT Hit0,
    _In_ PCINTERNAL_HIT Hit1
    )
{
    ASSERT(Hit0 != NULL);
    ASSERT(Hit1 != NULL);

    return (Hit0->Hit.Distance <= Hit1->Hit.Distance) ? -1 : 1;
}

#endif // _IRIS_HIT_ALLOCATOR_INTERNAL_