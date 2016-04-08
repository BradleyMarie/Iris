/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    hitallocator.cpp

Abstract:

    This file contains the definitions for the 
    IrisPysx++ HitAllocator type.

--*/

#include <irisphysxplusplusp.h>
namespace IrisPhysx {

HitAllocator::HitAllocator(
    _In_ PPBR_HIT_ALLOCATOR HitAllocatorPtr
    )
: Data(HitAllocatorPtr)
{ 
    if (HitAllocatorPtr == nullptr)
    {
        throw std::invalid_argument("HitAllocatorPtr");
    }
}

_Ret_
PHIT_LIST
HitAllocator::Allocate(
    _In_opt_ PHIT_LIST NextHit,
    _In_ FLOAT Distance,
    _In_ INT32 FaceHit,
    _In_reads_bytes_opt_(AdditionalDataSizeInBytes) PCVOID AdditionalData,
    _In_ SIZE_T AdditionalDataSizeInBytes,
    _In_ SIZE_T AdditionalDataAlignment
    )
{
    PHIT_LIST Result;
    
    ISTATUS Status = PBRHitAllocatorAllocate(Data,
                                             NextHit,
                                             Distance,
                                             FaceHit,
                                             AdditionalData,
                                             AdditionalDataSizeInBytes,
                                             AdditionalDataAlignment,
                                             &Result);

    if (Status != ISTATUS_SUCCESS)
    {
        throw std::runtime_error(Iris::ISTATUSToCString(Status));
    }
    
    return Result;
}

_Ret_
PHIT_LIST
HitAllocator::Allocate(
    _In_opt_ PHIT_LIST NextHit,
    _In_ FLOAT Distance,
    _In_ INT32 FaceHit,
    _In_reads_bytes_opt_(AdditionalDataSizeInBytes) PCVOID AdditionalData,
    _In_ SIZE_T AdditionalDataSizeInBytes,
    _In_ SIZE_T AdditionalDataAlignment,
    _In_ const Iris::Point & HitPoint
    )
{
    PHIT_LIST Result;
    
    ISTATUS Status = PBRHitAllocatorAllocateWithHitPoint(Data,
                                                         NextHit,
                                                         Distance,
                                                         FaceHit,
                                                         AdditionalData,
                                                         AdditionalDataSizeInBytes,
                                                         AdditionalDataAlignment,
                                                         HitPoint.AsPOINT3(),
                                                         &Result);

    if (Status != ISTATUS_SUCCESS)
    {
        throw std::runtime_error(Iris::ISTATUSToCString(Status));
    }
    
    return Result;
}

} // namespace IrisPhysx