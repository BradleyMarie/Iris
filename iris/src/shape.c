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
_Ret_maybenull_
PSHAPE
ShapeAllocate(
    _In_ PCSHAPE_VTABLE ShapeVTable,
    _Field_size_bytes_(DataSizeInBytes) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _In_ SIZE_T DataAlignment
    )
{
    PSHAPE Shape;
    SIZE_T DataOffset;
    PVOID DataAddress;
    SIZE_T AllocationSize;
    PVOID Allocation;
    ISTATUS Status;

    if (ShapeVTable == NULL ||
        Data == NULL ||
        DataSizeInBytes == 0 ||
        DataAlignment == 0)
    {
        return NULL;
    }

    Status = CheckedAddSizeT(sizeof(SHAPE),
                             DataAlignment,
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

    Allocation = malloc(AllocationSize);

    if (Allocation == NULL)
    {
        return NULL;
    }

    DataOffset = (SIZE_T) ((PUINT8) Allocation + sizeof(SHAPE)) % DataAlignment;

    if (DataOffset != 0)
    {
        DataOffset = DataAlignment - DataOffset;
    }

    DataOffset += sizeof(SHAPE);

    DataAddress = (PUINT8) Allocation + DataOffset;

    Shape = (PSHAPE) Allocation;

    Shape->VTable = ShapeVTable;
    Shape->ReferenceCount = 1;
    Shape->DataOffset = DataOffset;
    memcpy(DataAddress, Data, DataSizeInBytes);

    return Shape;
}

_Ret_maybenull_
PCVOID
ShapeGetData(
    _In_opt_ PCSHAPE Shape
    )
{
    if (Shape == NULL)
    {
        return NULL;
    }

    return (PUINT8) Shape + Shape->DataOffset;
}

_Ret_maybenull_
PCSHAPE_VTABLE
ShapeGetVTable(
    _In_opt_ PCSHAPE Shape
    )
{
    if (Shape == NULL)
    {
        return NULL;
    }

    return Shape->VTable;
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
    _Pre_maybenull_ _Post_invalid_ PSHAPE Shape
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