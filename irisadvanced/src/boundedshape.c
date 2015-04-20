/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    boundedshape.c

Abstract:

    This file contains the function definitions for the BOUNDED_SHAPE type.

--*/

#include <irisadvanced.h>

//
// Functions
//

_Check_return_
_Ret_maybenull_
PBOUNDED_SHAPE
BoundedShapeAllocate(
    _In_ PCBOUNDED_SHAPE_VTABLE BoundedShapeVTable,
    _In_reads_bytes_(DataSizeInBytes) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _In_ SIZE_T DataAlignment
    )
{
    PSHAPE Shape;

    Shape = ShapeAllocate(&BoundedShapeVTable->ShapeVTable,
                          Data,
                          DataSizeInBytes,
                          DataAlignment);

    return (PBOUNDED_SHAPE) Shape;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS 
BoundedShapeCheckBounds(
    _In_ PCBOUNDED_SHAPE BoundedShape,
    _In_ PCMATRIX ModelToWorld,
    _In_ BOUNDING_BOX WorldAlignedBoundingBox,
    _Out_ PBOOL IsInsideBox
    )
{
    PCBOUNDED_SHAPE_VTABLE BoundedShapeVTable;
    PCSHAPE_VTABLE ShapeVTable;
    ISTATUS Status;
    PCSHAPE Shape;
    PCVOID Data;

    if (BoundedShape == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (IsInsideBox == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    Shape = (PCSHAPE) BoundedShape;

    Data = ShapeGetData(Shape);

    ShapeVTable = ShapeGetVTable(Shape);

    BoundedShapeVTable = (PCBOUNDED_SHAPE_VTABLE) ShapeVTable;

    Status = BoundedShapeVTable->CheckBoundsRoutine(Data,
                                                    ModelToWorld,
                                                    WorldAlignedBoundingBox,
                                                    IsInsideBox);

    return Status;
}

VOID
BoundedShapeReference(
    _In_opt_ PBOUNDED_SHAPE BoundedShape
    )
{
    PSHAPE Shape;

    Shape = (PSHAPE) BoundedShape;

    ShapeReference(Shape);
}

VOID
BoundedShapeDereference(
    _In_opt_ _Post_invalid_ PBOUNDED_SHAPE BoundedShape
    )
{
    PSHAPE Shape;

    Shape = (PSHAPE) BoundedShape;

    ShapeDereference(Shape);
}