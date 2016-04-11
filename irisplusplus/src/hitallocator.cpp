/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    hitallocator.cpp

Abstract:

    This file contains the definitions for the 
    Iris++ HitAllocator type.

--*/

#include <irisplusplusp.h>

namespace Iris {

//
// Functions
//

_Ret_
PHIT_LIST
HitAllocator::Allocate(
    _In_opt_ PHIT_LIST NextHit,
    _In_ PCVOID DataPtr,
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
                                  DataPtr,
                                  Distance,
                                  FaceHit,
                                  AdditionalData,
                                  AdditionalDataSizeInBytes,
                                  AdditionalDataAlignment,
                                  &Result);

    if (Status == ISTATUS_SUCCESS)
    {
        return Result;
    }

    switch (Status)
    {
        case ISTATUS_ALLOCATION_FAILED:
            throw std::bad_alloc();
        default:
            throw std::runtime_error(ISTATUSToCString(Status));
    }
}

_Ret_
PHIT_LIST
HitAllocator::Allocate(
    _In_opt_ PHIT_LIST NextHit,
    _In_ PCVOID DataPtr,
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
                                              DataPtr,
                                              Distance,
                                              FaceHit,
                                              AdditionalData,
                                              AdditionalDataSizeInBytes,
                                              AdditionalDataAlignment,
                                              HitPoint,
                                              &Result);

    if (Status == ISTATUS_SUCCESS)
    {
        return Result;
    }

    switch (Status)
    {
        case ISTATUS_ALLOCATION_FAILED:
            throw std::bad_alloc();
        default:
            throw std::runtime_error(ISTATUSToCString(Status));
    }
}

} // namespace Iris