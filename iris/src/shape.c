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
_When_(DataSizeInBytes != 0 && Data != NULL && DataAlignment != 0, _Ret_opt_)
_When_(DataSizeInBytes != 0 && Data == NULL, _Ret_null_)
PSHAPE
ShapeAllocate(
    _In_ PCSHAPE_VTABLE ShapeVTable,
    _In_reads_bytes_opt_(DataSizeInBytes) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _In_ _When_(DataSizeInBytes == 0, _Reserved_) SIZE_T DataAlignment
    )
{
    PSHAPE Shape;
    SIZE_T DataOffset;
    PVOID DataAddress;
    SIZE_T AllocationSize;
    SIZE_T DataPadding;
    PVOID Allocation;
    ISTATUS Status;

    if (ShapeVTable == NULL)
    {
        return NULL;
    }

    if (DataSizeInBytes != 0)
    {
        if (Data == NULL ||
            DataAlignment == 0)
        {
            return NULL;
        }

        //
        // This technically means even-sized alignments
        // are 1 byte larger than they need to be.
        //
        
        if (DataAlignment % 2 == 1)
        {
            DataPadding = DataAlignment - 1;
        }
        else
        {
            DataPadding = DataAlignment;
        }

        Status = CheckedAddSizeT(sizeof(SHAPE),
                                 DataPadding,
                                 &AllocationSize);

        if (Status != ISTATUS_SUCCESS)
        {
            return NULL;
        }

        Status = CheckedAddSizeT(AllocationSize,
                                 DataSizeInBytes,
                                 &AllocationSize);

        if (Status != ISTATUS_SUCCESS)
        {
            return NULL;
        }
    }
    else
    {
        AllocationSize = sizeof(SHAPE);
    }

    Allocation = malloc(AllocationSize);

    if (Allocation == NULL)
    {
        return NULL;
    }

    Shape = (PSHAPE) Allocation;

    Shape->VTable = ShapeVTable;
    Shape->ReferenceCount = 1;

    if (DataSizeInBytes != 0)
    {
        DataOffset = (SIZE_T) ((PUINT8) Allocation + sizeof(SHAPE)) % DataAlignment;

        if (DataOffset != 0)
        {
            DataOffset = DataAlignment - DataOffset;
        }

        DataOffset += sizeof(SHAPE);

        DataAddress = (PUINT8) Allocation + DataOffset;

        Shape->DataOffset = DataOffset;
        memcpy(DataAddress, Data, DataSizeInBytes);
    }
    else
    {
        DataOffset = 0;
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
    if (Shape == NULL || Shape->DataOffset == 0)
    {
        return NULL;
    }

    return (PUINT8) Shape + Shape->DataOffset;
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
    PVOID DataAddress;

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
            DataAddress = (PUINT8) Shape +
                          Shape->DataOffset;

            FreeRoutine(DataAddress);
        }

        free(Shape);
    }
}