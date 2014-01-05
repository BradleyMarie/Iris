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
PSHAPE_HIT_LIST
ShapeHitAllocatorAllocate(
    _Inout_ PSHAPE_HIT_ALLOCATOR Allocator,
    _In_ PSHAPE_HIT_LIST NextShapeHit,
    _In_ PCSHAPE Shape,
    _In_ FLOAT Distance,
    _In_ INT32 FaceHit,
    _Field_size_bytes_opt_(AdditionalDataSizeInBytes) PCVOID AdditionalData,
    _In_ SIZE_T AdditionalDataSizeInBytes
    )
{
    PIRIS_DYNAMIC_MEMORY_ALLOCATOR AdditionalDataAllocator;
    PIRIS_STATIC_MEMORY_ALLOCATOR ShapeHitAllocator;
    PSHAPE_HIT_LIST ShapeHitList;
    PFULL_SHAPE_HIT FullShapeHit;
    PSHAPE_HIT ShapeHit;
    PVOID Allocation;

    ASSERT(AdditionalData == NULL || AdditionalDataSizeInBytes == 0);
    ASSERT(IsNormalFloat(Distance));
    ASSERT(IsFiniteFloat(Distance));
    ASSERT(Allocator != NULL);
    ASSERT(Shape != NULL);

    ShapeHitAllocator = &Allocator->ShapeHitAllocator;

    Allocation = IrisStaticMemoryAllocatorAllocate(ShapeHitAllocator);

    FullShapeHit = (PFULL_SHAPE_HIT) Allocation;

    ShapeHitList = &FullShapeHit->ShapeHitList;
	ShapeHit = &FullShapeHit->ShapeHit;

	if (AdditionalDataSizeInBytes != 0)
    {
        AdditionalDataAllocator = &Allocator->AdditionalDataAllocator;

        Allocation = IrisDynamicMemoryAllocatorAllocate(AdditionalDataAllocator,
                                                        AdditionalDataSizeInBytes);

        if (Allocation == NULL)
        {
            IrisStaticMemoryAllocatorFreeLastAllocation(ShapeHitAllocator);
            return NULL;
        }

        memcpy(Allocation, AdditionalData, AdditionalDataSizeInBytes);
    }
    else
    {
        Allocation = NULL;
    }

    ShapeHit->Shape = Shape;
    ShapeHit->Distance = Distance;
    ShapeHit->FaceHit = FaceHit;
	ShapeHit->AdditionalDataSizeInBytes = AdditionalDataSizeInBytes;
    ShapeHit->AdditionalData = Allocation;

    ShapeHitList->NextShapeHit = NextShapeHit;
    ShapeHitList->ShapeHit = ShapeHit;

    return ShapeHitList;
}