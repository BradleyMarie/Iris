/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    shape.c

Abstract:

    This file contains the function definitions for the SHAPE type.

--*/

#include <irisp.h>

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
ShapeAllocate(
    _In_ PCSHAPE_VTABLE ShapeVTable,
    _When_(DataSizeInBytes != 0, _In_reads_bytes_opt_(DataSizeInBytes)) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _When_(DataSizeInBytes != 0, _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && DataSizeInBytes % _Curr_ == 0)) SIZE_T DataAlignment,
    _Out_ PSHAPE *Shape
    )
{
    BOOL AllocationSuccessful;
    PVOID HeaderAllocation;
    PVOID DataAllocation;
    PSHAPE AllocatedShape;

    if (ShapeVTable == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (DataSizeInBytes != 0)
    {
        if (Data == NULL)
        {
            return ISTATUS_INVALID_ARGUMENT_COMBINATION_00;
        }

        if (DataAlignment == 0 ||
            DataAlignment & DataAlignment - 1)
        {
            return ISTATUS_INVALID_ARGUMENT_COMBINATION_01;
        }

        if (DataSizeInBytes % DataAlignment != 0)
        {
            return ISTATUS_INVALID_ARGUMENT_COMBINATION_02;
        }
    }

    if (Shape == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    AllocationSuccessful = IrisAlignedAllocWithHeader(sizeof(SHAPE),
                                                      _Alignof(SHAPE),
                                                      &HeaderAllocation,
                                                      DataSizeInBytes,
                                                      DataAlignment,
                                                      &DataAllocation,
                                                      NULL);

    if (AllocationSuccessful == FALSE)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    AllocatedShape = (PSHAPE) HeaderAllocation;

    ShapeReferenceInitialize(ShapeVTable,
                             DataAllocation,
                             &AllocatedShape->ShapeReference);
                             
    AllocatedShape->ReferenceCount = 1;

    if (DataSizeInBytes != 0)
    {
        memcpy(DataAllocation, Data, DataSizeInBytes);
    }

    *Shape = AllocatedShape;

    return ISTATUS_SUCCESS;
}

_Ret_
PCSHAPE_VTABLE
ShapeGetVTable(
    _In_ PCSHAPE Shape
    )
{
    PCSHAPE_VTABLE Result;
    
    if (Shape == NULL)
    {
        return NULL;
    }

    Result = ShapeReferenceGetVTable(&Shape->ShapeReference);

    return Result;
}

_Ret_
PCVOID
ShapeGetData(
    _In_ PCSHAPE Shape
    )
{
    PCVOID Result;
    
    if (Shape == NULL)
    {
        return NULL;
    }

    Result = ShapeReferenceGetData(&Shape->ShapeReference);

    return Result;
}

VOID
ShapeRetain(
    _In_opt_ PSHAPE Shape
    )
{
    if (Shape == NULL)
    {
        return;
    }

    Shape->ReferenceCount += 1;
}

VOID
ShapeRelease(
    _In_opt_ _Post_invalid_ PSHAPE Shape
    )
{
    if (Shape == NULL)
    {
        return;
    }

    Shape->ReferenceCount -= 1;

    if (Shape->ReferenceCount == 0)
    {
        ShapeReferenceDestroy(&Shape->ShapeReference);
        IrisAlignedFree(Shape);
    }
}