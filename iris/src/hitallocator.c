/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    hitallocator.c

Abstract:

    This module implements the HIT_ALLOCATOR functions.

--*/

#include <irisp.h>

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
HitAllocatorAllocateInternal(
    _Inout_ PHIT_ALLOCATOR Allocator,
    _In_opt_ PHIT_LIST NextHit,
    _In_ FLOAT Distance,
    _In_ INT32 FaceHit,
    _In_reads_bytes_opt_(AdditionalDataSizeInBytes) PCVOID AdditionalData,
    _In_ SIZE_T AdditionalDataSizeInBytes,
    _In_ SIZE_T AdditionalDataAlignment,
    _In_opt_ PPOINT3 ModelHitPoint,
    _Out_ PHIT_LIST *OutputHitList
    )
{
    PHIT_ALLOCATOR_ALLOCATION HitAllocatorAllocation;
    PDYNAMIC_MEMORY_ALLOCATOR AdditionalDataAllocator;
    PSTATIC_MEMORY_ALLOCATOR HitAllocator;
    PINTERNAL_HIT InternalHit;
    PHIT_LIST HitList;
    PHIT Hit;
    PVOID Allocation;

    ASSERT(OutputHitList != NULL);

    if (Allocator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (IsFiniteFloat(Distance) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (AdditionalDataSizeInBytes != 0)
    {
        if (AdditionalData == NULL)
        {
            return ISTATUS_INVALID_ARGUMENT_COMBINATION_00;
        }
        
        if (AdditionalDataAlignment == 0 ||
            AdditionalDataAlignment & AdditionalDataAlignment - 1)
        {
            return ISTATUS_INVALID_ARGUMENT_COMBINATION_01;    
        }
        
        if (AdditionalDataSizeInBytes % AdditionalDataAlignment != 0)
        {
            return ISTATUS_INVALID_ARGUMENT_COMBINATION_02;
        }
    }

    HitAllocator = &Allocator->HitAllocator;

    Allocation = StaticMemoryAllocatorAllocate(HitAllocator);

    if (Allocation == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    HitAllocatorAllocation = (PHIT_ALLOCATOR_ALLOCATION) Allocation;

    if (AdditionalDataSizeInBytes != 0)
    {
        AdditionalDataAllocator = &Allocator->AdditionalDataAllocator;

        Allocation = DynamicMemoryAllocatorAllocate(AdditionalDataAllocator,
                                                    AdditionalDataSizeInBytes,
                                                    AdditionalDataAlignment);

        if (Allocation == NULL)
        {
            StaticMemoryAllocatorFreeLastAllocation(HitAllocator);
            return ISTATUS_ALLOCATION_FAILED;
        }

        memcpy(Allocation, AdditionalData, AdditionalDataSizeInBytes);
    }
    else
    {
        Allocation = NULL;
    }

    HitList = &HitAllocatorAllocation->HitList;
    InternalHit = &HitAllocatorAllocation->InternalHit;
    Hit = &InternalHit->Hit;

    Hit->ShapeReference = Allocator->CurrentShapeReference;
    Hit->Distance = Distance;
    Hit->FaceHit = FaceHit;
    Hit->AdditionalDataSizeInBytes = AdditionalDataSizeInBytes;
    Hit->AdditionalData = Allocation;

    if (ModelHitPoint != NULL)
    {
        InternalHit->ModelHitPoint = *ModelHitPoint;
        InternalHit->ModelHitPointValid = TRUE;
    }
    else
    {
        InternalHit->ModelHitPointValid = FALSE;
    }

    HitList->NextHit = NextHit;
    HitList->Hit = Hit;

    *OutputHitList = HitList;

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
HitAllocatorAllocate(
    _Inout_ PHIT_ALLOCATOR HitAllocator,
    _In_opt_ PHIT_LIST NextHit,
    _In_ FLOAT Distance,
    _In_ INT32 FaceHit,
    _In_reads_bytes_opt_(AdditionalDataSizeInBytes) PCVOID AdditionalData,
    _In_ SIZE_T AdditionalDataSizeInBytes,
    _In_ SIZE_T AdditionalDataAlignment,
    _Out_ PHIT_LIST *HitList
    )
{
    ISTATUS Status;

    if (HitList == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_06;
    }

    Status = HitAllocatorAllocateInternal(HitAllocator,
                                          NextHit,
                                          Distance,
                                          FaceHit,
                                          AdditionalData,
                                          AdditionalDataSizeInBytes,
                                          AdditionalDataAlignment,
                                          NULL,
                                          HitList);

    return Status;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
HitAllocatorAllocateWithHitPoint(
    _Inout_ PHIT_ALLOCATOR HitAllocator,
    _In_opt_ PHIT_LIST NextHit,
    _In_ FLOAT Distance,
    _In_ INT32 FaceHit,
    _In_reads_bytes_opt_(AdditionalDataSizeInBytes) PCVOID AdditionalData,
    _In_ SIZE_T AdditionalDataSizeInBytes,
    _In_ SIZE_T AdditionalDataAlignment,
    _In_ POINT3 HitPoint,
    _Out_ PHIT_LIST *HitList
    )
{
    ISTATUS Status;

    if (PointValidate(HitPoint) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_06;
    }

    if (HitList == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_07;
    }

    Status = HitAllocatorAllocateInternal(HitAllocator,
                                          NextHit,
                                          Distance,
                                          FaceHit,
                                          AdditionalData,
                                          AdditionalDataSizeInBytes,
                                          AdditionalDataAlignment,
                                          &HitPoint,
                                          HitList);

    return Status;
}