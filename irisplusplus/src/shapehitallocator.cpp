/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    shapehitallocator.cpp

Abstract:

    This file contains the definitions for the 
    Iris++ shapehitallocator type.

--*/

#include <irisplusplus.h>

namespace Iris {

//
// Functions
//

_Ret_
ShapeHitList *
ShapeHitAllocator::Allocate(
    _In_opt_ ShapeHitList * NextShapeHit,
    _In_ FLOAT Distance,
    _In_ INT32 FaceHit,    
    _In_reads_bytes_opt_(AdditionalDataSizeInBytes) PCVOID AdditionalData,
    _In_ SIZE_T AdditionalDataSizeInBytes
    )
{
    PSHAPE_HIT_LIST Result;
    ISTATUS Status;

    Status = ShapeHitAllocatorAllocate(Data,
                                       NextShapeHit,
                                       Distance,
                                       FaceHit,
                                       AdditionalData,
                                       AdditionalDataSizeInBytes,
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
ShapeHitList *
ShapeHitAllocator::Allocate(
    _In_opt_ ShapeHitList * NextShapeHit,
    _In_ FLOAT Distance,
    _In_ INT32 FaceHit,
    _In_ POINT3 & HitPoint,
    _In_reads_bytes_opt_(AdditionalDataSizeInBytes) PCVOID AdditionalData,
    _In_ SIZE_T AdditionalDataSizeInBytes
    )
{
    PSHAPE_HIT_LIST Result;
    ISTATUS Status;

    Status = ShapeHitAllocatorAllocateWithHitPoint(Data,
                                                   NextShapeHit,
                                                   Distance,
                                                   FaceHit,
                                                   AdditionalData,
                                                   AdditionalDataSizeInBytes,
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