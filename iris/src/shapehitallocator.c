/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    shapehitallocator.c

Abstract:

    This module implements the SHAPE_HIT_ALLOCATOR functions.

--*/

#include <irisp.h>

_Check_return_
_Ret_opt_
SFORCEINLINE
PSHAPE_HIT_LIST
ShapeHitAllocatorAllocateInternal(
    _Inout_ PSHAPE_HIT_ALLOCATOR Allocator,
    _In_opt_ PSHAPE_HIT_LIST NextShapeHit,
    _In_ FLOAT Distance,
    _In_ INT32 FaceHit,
    _In_reads_bytes_opt_(AdditionalDataSizeInBytes) PCVOID AdditionalData,
    _In_ SIZE_T AdditionalDataSizeInBytes,
    _In_opt_ PPOINT3 ModelHitPoint
    )
{
    PSHAPE_HIT_ALLOCATOR_ALLOCATION ShapeHitAllocatorAllocation;
    PIRIS_DYNAMIC_MEMORY_ALLOCATOR AdditionalDataAllocator;
    PIRIS_STATIC_MEMORY_ALLOCATOR ShapeHitAllocator;
    PINTERNAL_SHAPE_HIT InternalShapeHit;
    PSHAPE_HIT_LIST ShapeHitList;
    PSHAPE_HIT ShapeHit;
    PVOID Allocation;

    if (Allocator == NULL ||
        IsNormalFloat(Distance) == FALSE || IsFiniteFloat(Distance) == FALSE ||
        (AdditionalData == NULL && AdditionalDataSizeInBytes != 0))
    {
        return NULL;
    }

    ShapeHitAllocator = &Allocator->ShapeHitAllocator;

    Allocation = IrisStaticMemoryAllocatorAllocate(ShapeHitAllocator);

    if (Allocation == NULL)
    {
        return NULL;
    }

    ShapeHitAllocatorAllocation = (PSHAPE_HIT_ALLOCATOR_ALLOCATION) Allocation;

    ShapeHitList = &ShapeHitAllocatorAllocation->ShapeHitList;
    InternalShapeHit = &ShapeHitAllocatorAllocation->InternalShapeHit;
    ShapeHit = &InternalShapeHit->ShapeHit;

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

    ShapeHit->Shape = Allocator->CurrentShape;
    ShapeHit->Distance = Distance;
    ShapeHit->FaceHit = FaceHit;
    ShapeHit->AdditionalDataSizeInBytes = AdditionalDataSizeInBytes;
    ShapeHit->AdditionalData = Allocation;

    if (ModelHitPoint != NULL)
    {
        InternalShapeHit->ModelHitPoint = *ModelHitPoint;
        InternalShapeHit->ModelHitPointValid = TRUE;
    }
    else
    {
        InternalShapeHit->ModelHitPointValid = FALSE;
    }

    ShapeHitList->NextShapeHit = NextShapeHit;
    ShapeHitList->ShapeHit = ShapeHit;

    return ShapeHitList;
}

_Check_return_
_Ret_opt_
PSHAPE_HIT_LIST
ShapeHitAllocatorAllocate(
    _Inout_ PSHAPE_HIT_ALLOCATOR ShapeHitAllocator,
    _In_opt_ PSHAPE_HIT_LIST NextShapeHit,
    _In_ FLOAT Distance,
    _In_ INT32 FaceHit,
    _In_reads_bytes_opt_(AdditionalDataSizeInBytes) PCVOID AdditionalData,
    _In_ SIZE_T AdditionalDataSizeInBytes
    )
{
    PSHAPE_HIT_LIST ShapeHitList;

    ShapeHitList = ShapeHitAllocatorAllocateInternal(ShapeHitAllocator,
                                                     NextShapeHit,
                                                     Distance,
                                                     FaceHit,
                                                     AdditionalData,
                                                     AdditionalDataSizeInBytes,
                                                     NULL);

    return ShapeHitList;
}

_Check_return_
_Ret_opt_
PSHAPE_HIT_LIST
ShapeHitAllocatorAllocateWithHitPoint(
    _Inout_ PSHAPE_HIT_ALLOCATOR ShapeHitAllocator,
    _In_opt_ PSHAPE_HIT_LIST NextShapeHit,
    _In_ FLOAT Distance,
    _In_ INT32 FaceHit,
    _In_reads_bytes_opt_(AdditionalDataSizeInBytes) PCVOID AdditionalData,
    _In_ SIZE_T AdditionalDataSizeInBytes,
    _In_ POINT3 HitPoint
    )
{
    PSHAPE_HIT_LIST ShapeHitList;

    ShapeHitList = ShapeHitAllocatorAllocateInternal(ShapeHitAllocator,
                                                     NextShapeHit,
                                                     Distance,
                                                     FaceHit,
                                                     AdditionalData,
                                                     AdditionalDataSizeInBytes,
                                                     &HitPoint);

    return ShapeHitList;
}