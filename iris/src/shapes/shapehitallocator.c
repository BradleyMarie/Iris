/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    shapehitallocator.c

Abstract:

    This module implements the SHAPE_HIT_ALLOCATOR functions.

--*/

#include <irisp.h>

_Check_return_
_Ret_maybenull_
PSHAPE_HIT
ShapeHitAllocatorAllocate(
    _Inout_ PSHAPE_HIT_ALLOCATOR ShapeHitAllocator,
    _In_ PSHAPE_HIT FartherHit,
    _In_ PSHAPE Shape,
    _In_ FLOAT Distance,
    _In_ INT32 FaceHit,
    _In_ SIZE_T AdditionalDataSize
    )
{
    PIRIS_DYNAMIC_MEMORY_ALLOCATOR AdditionalDataAllocator;
    PIRIS_STATIC_MEMORY_ALLOCATOR GeometryHitAllocator;
    PGEOMETRY_HIT GeometryHit;
    PSHAPE_HIT ShapeHit;
    PVOID Allocation;

    ASSERT(ShapeHitAllocator != NULL);
    ASSERT(Shape != NULL);
    ASSERT(IsNormalFloat(Distance));
    ASSERT(IsFiniteFloat(Distance));

    GeometryHitAllocator = &ShapeHitAllocator->GeometryHitAllocator;

    Allocation = IrisStaticMemoryAllocatorAllocate(GeometryHitAllocator);

    GeometryHit = (PGEOMETRY_HIT) Allocation;

    if (AdditionalDataSize != 0)
    {
        AdditionalDataAllocator = &ShapeHitAllocator->AdditionalDataAllocator;

        Allocation = IrisDynamicMemoryAllocatorAllocate(AdditionalDataAllocator,
                                                        AdditionalDataSize);

        if (Allocation == NULL)
        {
            IrisStaticMemoryAllocatorFreeLastAllocation(GeometryHitAllocator);
            return NULL;
        }  
    }
    else
    {
        Allocation = NULL;
    }

    ShapeHit = &GeometryHit->ShapeHit;

    ShapeHit->Distance = Distance;
    ShapeHit->FaceHit = FaceHit;
    ShapeHit->AdditionalDataSizeInBytes = AdditionalDataSize;
    ShapeHit->AdditionalData = Allocation;

    return ShapeHit;
}