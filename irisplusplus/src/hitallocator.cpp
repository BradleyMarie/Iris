/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    hitallocator.cpp

Abstract:

    This file contains the definitions for the 
    Iris++ HitAllocator type.

--*/

#include <irisplusplus.h>

namespace Iris {

//
// Functions
//

_Ret_
PHIT_LIST
ShapeBase::HitAllocator::Allocate(
    _In_opt_ PHIT_LIST NextHit,
    _In_ FLOAT Distance,
    _In_ INT32 FaceHit,    
    _In_reads_bytes_opt_(AdditionalDataSizeInBytes) PCVOID AdditionalData,
    _In_ SIZE_T AdditionalDataSizeInBytes,
    _In_ SIZE_T AdditionalDataAlignment
    )
{
    PHIT_LIST Result;
    ISTATUS Status;

    Status = HitAllocatorAllocate(Data,
                                  NextHit,
                                  Distance,
                                  FaceHit,
                                  AdditionalData,
                                  AdditionalDataSizeInBytes,
                                  AdditionalDataAlignment,
                                  &Result);

    switch (Status)
    {
        case ISTATUS_SUCCESS:
            break;
        case ISTATUS_ALLOCATION_FAILED:
            throw std::bad_alloc();
            break;
        case ISTATUS_INVALID_ARGUMENT_02:
            throw std::invalid_argument("Distance");
            break;
        default:
            ASSERT(FALSE);
    }

    return Result;
}

_Ret_
PHIT_LIST
ShapeBase::HitAllocator::Allocate(
    _In_opt_ PHIT_LIST NextHit,
    _In_ FLOAT Distance,
    _In_ INT32 FaceHit,
    _In_ const POINT3 & HitPoint,
    _In_reads_bytes_opt_(AdditionalDataSizeInBytes) PCVOID AdditionalData,
    _In_ SIZE_T AdditionalDataSizeInBytes,
    _In_ SIZE_T AdditionalDataAlignment
    )
{
    PHIT_LIST Result;
    ISTATUS Status;

    Status = HitAllocatorAllocateWithHitPoint(Data,
                                              NextHit,
                                              Distance,
                                              FaceHit,
                                              AdditionalData,
                                              AdditionalDataSizeInBytes,
                                              AdditionalDataAlignment,
                                              HitPoint,
                                              &Result);

    switch (Status)
    {
        case ISTATUS_SUCCESS:
            break;
        case ISTATUS_ALLOCATION_FAILED:
            throw std::bad_alloc();
            break;
        case ISTATUS_INVALID_ARGUMENT_02:
            throw std::invalid_argument("Distance");
            break;
        default:
            ASSERT(FALSE);
    }

    return Result;
}

} // namespace Iris