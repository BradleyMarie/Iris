/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    advancedshape.c

Abstract:

    This file contains the function definitions for the ADVANCED_SHAPE type.

--*/

#include <irisadvancedp.h>

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
AdvancedShapeAllocate(
    _In_ PCADVANCED_SHAPE_VTABLE AdvancedShapeVTable,
    _When_(DataSizeInBytes != 0, _In_reads_bytes_opt_(DataSizeInBytes)) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _When_(DataSizeInBytes != 0, _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && DataSizeInBytes % _Curr_ == 0)) SIZE_T DataAlignment,
    _Out_ PADVANCED_SHAPE *AdvancedShape
    )
{
	ISTATUS Status;
	PSHAPE *Shape;

	Shape = (PSHAPE *) AdvancedShape;

	Status = ShapeAllocate(&AdvancedShapeVTable->ShapeVTable,
                           Data,
                           DataSizeInBytes,
                           DataAlignment,
						   Shape);

    return Status;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISADVANCEDAPI
ISTATUS 
AdvancedShapeComputeNormal(
    _In_ PCADVANCED_SHAPE AdvancedShape,
    _In_ POINT3 ModelHitPoint,
    _In_ UINT32 FaceHit,
    _Out_ PVECTOR3 SurfaceNormal
    )
{
    PCADVANCED_SHAPE_VTABLE AdvancedShapeVTable;
    PCSHAPE_VTABLE ShapeVTable;
    ISTATUS Status;
    PCSHAPE Shape;
    PCVOID Data;

    if (AdvancedShape == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (SurfaceNormal == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    Shape = (PCSHAPE) AdvancedShape;

    Data = ShapeGetData(Shape);

    ShapeVTable = ShapeGetVTable(Shape);

    AdvancedShapeVTable = (PCADVANCED_SHAPE_VTABLE) ShapeVTable;

    Status = AdvancedShapeVTable->ComputeShapeNormalRoutine(Data,
                                                            ModelHitPoint,
                                                            FaceHit,
                                                            SurfaceNormal);

    return Status;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS 
AdvancedShapeCheckBounds(
    _In_ PCADVANCED_SHAPE AdvancedShape,
    _In_ PCMATRIX ModelToWorld,
    _In_ BOUNDING_BOX WorldAlignedBoundingBox,
    _Out_ PBOOL IsInsideBox
    )
{
    PCADVANCED_SHAPE_VTABLE AdvancedShapeVTable;
    PCSHAPE_VTABLE ShapeVTable;
    ISTATUS Status;
    PCSHAPE Shape;
    PCVOID Data;

    if (AdvancedShape == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (IsInsideBox == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    Shape = (PCSHAPE) AdvancedShape;

    Data = ShapeGetData(Shape);

    ShapeVTable = ShapeGetVTable(Shape);

    AdvancedShapeVTable = (PCADVANCED_SHAPE_VTABLE) ShapeVTable;

    Status = AdvancedShapeVTable->CheckBoundsRoutine(Data,
                                                     ModelToWorld,
                                                     WorldAlignedBoundingBox,
                                                     IsInsideBox);

    return Status;
}

VOID
AdvancedShapeReference(
    _In_opt_ PADVANCED_SHAPE AdvancedShape
    )
{
    PSHAPE Shape;

    Shape = (PSHAPE) AdvancedShape;

    ShapeReference(Shape);
}

VOID
AdvancedShapeDereference(
    _In_opt_ _Post_invalid_ PADVANCED_SHAPE AdvancedShape
    )
{
    PSHAPE Shape;

    Shape = (PSHAPE) AdvancedShape;

    ShapeDereference(Shape);
}