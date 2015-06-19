/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    shapehitallocator.c

Abstract:

    This module implements the SHAPE_HIT_ALLOCATOR functions.

--*/

#include <irisp.h>

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
ShapeHitAllocatorAllocateInternal(
    _Inout_ PSHAPE_HIT_ALLOCATOR Allocator,
    _In_opt_ PSHAPE_HIT_LIST NextShapeHit,
    _In_ FLOAT Distance,
    _In_ INT32 FaceHit,
    _In_reads_bytes_opt_(AdditionalDataSizeInBytes) PCVOID AdditionalData,
    _In_ SIZE_T AdditionalDataSizeInBytes,
    _In_opt_ PPOINT3 ModelHitPoint,
    _Out_ PSHAPE_HIT_LIST *OutputShapeHitList
    )
{
    PSHAPE_HIT_ALLOCATOR_ALLOCATION ShapeHitAllocatorAllocation;
    PDYNAMIC_MEMORY_ALLOCATOR AdditionalDataAllocator;
    PSTATIC_MEMORY_ALLOCATOR ShapeHitAllocator;
    PINTERNAL_SHAPE_HIT InternalShapeHit;
    PSHAPE_HIT_LIST ShapeHitList;
    PSHAPE_HIT ShapeHit;
    PVOID Allocation;

    if (Allocator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (IsNormalFloat(Distance) == FALSE ||
        IsFiniteFloat(Distance) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (AdditionalData == NULL &&
        AdditionalDataSizeInBytes != 0)
    {
        return ISTATUS_INVALID_ARGUMENT_COMBINATION_00;
    }

    ASSERT(OutputShapeHitList != NULL);

    ShapeHitAllocator = &Allocator->ShapeHitAllocator;

    Allocation = StaticMemoryAllocatorAllocate(ShapeHitAllocator);

    if (Allocation == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    ShapeHitAllocatorAllocation = (PSHAPE_HIT_ALLOCATOR_ALLOCATION) Allocation;

    ShapeHitList = &ShapeHitAllocatorAllocation->ShapeHitList;
    InternalShapeHit = &ShapeHitAllocatorAllocation->InternalShapeHit;
    ShapeHit = &InternalShapeHit->ShapeHit;

    if (AdditionalDataSizeInBytes != 0)
    {
        AdditionalDataAllocator = &Allocator->AdditionalDataAllocator;

        Allocation = DynamicMemoryAllocatorAllocate(AdditionalDataAllocator,
                                                    AdditionalDataSizeInBytes);

        if (Allocation == NULL)
        {
            StaticMemoryAllocatorFreeLastAllocation(ShapeHitAllocator);
            return ISTATUS_ALLOCATION_FAILED;
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

    *OutputShapeHitList = ShapeHitList;

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
ShapeHitAllocatorAllocate(
    _Inout_ PSHAPE_HIT_ALLOCATOR ShapeHitAllocator,
    _In_opt_ PSHAPE_HIT_LIST NextShapeHit,
    _In_ FLOAT Distance,
    _In_ INT32 FaceHit,
    _In_reads_bytes_opt_(AdditionalDataSizeInBytes) PCVOID AdditionalData,
    _In_ SIZE_T AdditionalDataSizeInBytes,
    _Out_ PSHAPE_HIT_LIST *ShapeHitList
    )
{
    ISTATUS Status;

    if (ShapeHitList == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_06;
    }

    Status = ShapeHitAllocatorAllocateInternal(ShapeHitAllocator,
                                               NextShapeHit,
                                               Distance,
                                               FaceHit,
                                               AdditionalData,
                                               AdditionalDataSizeInBytes,
                                               NULL,
                                               ShapeHitList);

    return Status;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
ShapeHitAllocatorAllocateWithHitPoint(
    _Inout_ PSHAPE_HIT_ALLOCATOR ShapeHitAllocator,
    _In_opt_ PSHAPE_HIT_LIST NextShapeHit,
    _In_ FLOAT Distance,
    _In_ INT32 FaceHit,
    _In_reads_bytes_opt_(AdditionalDataSizeInBytes) PCVOID AdditionalData,
    _In_ SIZE_T AdditionalDataSizeInBytes,
    _In_ POINT3 HitPoint,
    _Out_ PSHAPE_HIT_LIST *ShapeHitList
    )
{
    ISTATUS Status;

    if (ShapeHitList == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_07;
    }

    Status = ShapeHitAllocatorAllocateInternal(ShapeHitAllocator,
                                               NextShapeHit,
                                               Distance,
                                               FaceHit,
                                               AdditionalData,
                                               AdditionalDataSizeInBytes,
                                               &HitPoint,
                                               ShapeHitList);

    return Status;
}