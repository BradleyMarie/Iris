/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    shapebase.cpp

Abstract:

    This file contains the definitions for the 
    Iris++ ShapeBase type.

--*/

#include <irisplusplusp.h>

namespace Iris {

//
// HitAllocator Functions
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

//
// Static Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS 
ShapeBase::Test(
    _In_opt_ PCVOID Context, 
    _In_ RAY Ray,
    _Inout_ PHIT_ALLOCATOR IrisHitAllocator,
    _Outptr_result_maybenull_ PHIT_LIST *HitList
    )
{
    assert(Context != nullptr);
    assert(IrisHitAllocator != nullptr);
    assert(HitList != nullptr);

    HitAllocator Allocator(IrisHitAllocator);

    const ShapeBase **ShapeBasePointer = (const ShapeBase**) Context;
    *HitList = (*ShapeBasePointer)->Test(Ray, Allocator);
    return ISTATUS_SUCCESS;
}

VOID
ShapeBase::Free(
    _In_ _Post_invalid_ PVOID Context
    )
{
    assert(Context != nullptr);

    ShapeBase **ShapeBasePointer = (ShapeBase**) Context;
    delete *ShapeBasePointer;
}

Shape
ShapeBase::Create(
    _In_ std::unique_ptr<ShapeBase> ShapeBasePtr
    )
{
    if (!ShapeBasePtr)
    {
        throw std::invalid_argument("ShapeBasePtr");    
    }
    
    ShapeBase *UnmananagedShapeBasePtr = ShapeBasePtr.release();
    PSHAPE ShapePtr;

    ISTATUS Success = ShapeAllocate(&InteropVTable,
                                    &UnmananagedShapeBasePtr,
                                    sizeof(ShapeBase*),
                                    alignof(ShapeBase*),
                                    &ShapePtr);

    if (Success != ISTATUS_SUCCESS)
    {
        throw std::bad_alloc();
    }
    
    return Shape(ShapePtr, false);
}

//
// Static Variables
//

const SHAPE_VTABLE ShapeBase::InteropVTable = {
    ShapeBase::Test, ShapeBase::Free
};

} // namespace Iris