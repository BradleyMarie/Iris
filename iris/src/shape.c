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
_Ret_opt_
PSHAPE
ShapeAllocate(
    _In_ PCSHAPE_VTABLE ShapeVTable,
    _In_reads_bytes_opt_(DataSizeInBytes) PCVOID Data,
    _In_ _When_(Data == NULL, _Reserved_) SIZE_T DataSizeInBytes,
    _In_ SIZE_T DataAlignment
    )
{
    PSHAPE Shape;
    PVOID HeaderAllocation;
    PVOID DataAllocation;

    if (ShapeVTable == NULL)
    {
        return NULL;
    }

    if (Data == NULL && DataSizeInBytes != 0)
    {
        return NULL;
    }

    HeaderAllocation = IrisAlignedMallocWithHeader(sizeof(SHAPE),
                                                   sizeof(PVOID),
                                                   DataSizeInBytes,
                                                   DataAlignment,
                                                   &DataAllocation);

    if (HeaderAllocation == NULL)
    {
        return NULL;
    }

    Shape = (PSHAPE) HeaderAllocation;

    Shape->VTable = ShapeVTable;
    Shape->ReferenceCount = 1;
    Shape->Data = DataAllocation;

    if (DataSizeInBytes != 0)
    {
        memcpy(DataAllocation, Data, DataSizeInBytes);
    }

    return Shape;
}

_Ret_
PCSHAPE_VTABLE
ShapeGetVTable(
    _In_ PCSHAPE Shape
    )
{
    if (Shape == NULL)
    {
        return NULL;
    }

    return Shape->VTable;
}

_Check_return_
_Ret_opt_
PCVOID
ShapeGetData(
    _In_ PCSHAPE Shape
    )
{
    if (Shape == NULL)
    {
        return NULL;
    }

    return Shape->Data;
}

VOID
ShapeReference(
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
ShapeDereference(
    _In_opt_ _Post_invalid_ PSHAPE Shape
    )
{
    PFREE_ROUTINE FreeRoutine;

    if (Shape == NULL)
    {
        return;
    }

    Shape->ReferenceCount -= 1;

    if (Shape->ReferenceCount == 0)
    {
        FreeRoutine = Shape->VTable->FreeRoutine;

        if (FreeRoutine != NULL)
        {
            FreeRoutine(Shape->Data);
        }

        IrisAlignedFree(Shape);
    }
}