/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    hitallocator.c

Abstract:

    This file contains the definitions for the PBR_HIT_ALLOCATOR type.

--*/

#include <irisphysxp.h>

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PBRHitAllocatorAllocate(
    _Inout_ PPBR_HIT_ALLOCATOR PBRHitAllocator,
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
    
    Status = HitAllocatorAllocate(PBRHitAllocator->Allocator,
                                  NextHit,
                                  PBRHitAllocator->Geometry,
                                  Distance,
                                  FaceHit,
                                  AdditionalData,
                                  AdditionalDataSizeInBytes,
                                  AdditionalDataAlignment,
                                  HitList);
 
    if (Status == ISTATUS_SUCCESS)
    {
        return ISTATUS_SUCCESS;
    }
 
    if (ISTATUS_INVALID_ARGUMENT_03 <= Status &&
        Status <= ISTATUS_INVALID_ARGUMENT_08)
    {
        Status -= 1;
    }

    return Status;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PBRHitAllocatorAllocateWithHitPoint(
    _Inout_ PPBR_HIT_ALLOCATOR PBRHitAllocator,
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
    
    Status = HitAllocatorAllocateWithHitPoint(PBRHitAllocator->Allocator,
                                              NextHit,
                                              PBRHitAllocator->Geometry,
                                              Distance,
                                              FaceHit,
                                              AdditionalData,
                                              AdditionalDataSizeInBytes,
                                              AdditionalDataAlignment,
                                              HitPoint,
                                              HitList);
 
    if (Status == ISTATUS_SUCCESS)
    {
        return ISTATUS_SUCCESS;
    }
 
    if (ISTATUS_INVALID_ARGUMENT_03 <= Status &&
        Status <= ISTATUS_INVALID_ARGUMENT_09)
    {
        Status -= 1;
    }

    return Status;
}