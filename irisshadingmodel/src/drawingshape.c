/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    drawingshape.c

Abstract:

    This file contains the function definitions for the DRAWING_SHAPE type.

--*/

#include <irisshadingmodelp.h>

//
// Functions
//

_Check_return_
_Ret_opt_
PDRAWING_SHAPE
DrawingShapeAllocate(
    _In_ PCDRAWING_SHAPE_VTABLE DrawingShapeVTable,
    _In_reads_bytes_opt_(DataSizeInBytes) PCVOID Data,
    _In_ _When_(Data == NULL, _Reserved_) SIZE_T DataSizeInBytes,
    _In_ SIZE_T DataAlignment
    )
{
    ISTATUS Status;
    PSHAPE Shape;

    Status = ShapeAllocate(&DrawingShapeVTable->ShapeVTable,
                           Data,
                           DataSizeInBytes,
                           DataAlignment,
                           &Shape);

    return (PDRAWING_SHAPE) Shape;
}

_Ret_opt_
PCTEXTURE
DrawingShapeGetTexture(
    _In_ PCDRAWING_SHAPE DrawingShape,
    _In_ UINT32 FaceHit
    )
{
    PCDRAWING_SHAPE_VTABLE DrawingShapeVTable;
    PCSHAPE_VTABLE ShapeVTable;
    PCTEXTURE Texture;
    PCSHAPE Shape;
    PCVOID Data;

    if (DrawingShape == NULL)
    {
        return NULL;
    }

    Shape = (PCSHAPE) DrawingShape;

    Data = ShapeGetData(Shape);

    ShapeVTable = ShapeGetVTable(Shape);

    DrawingShapeVTable = (PCDRAWING_SHAPE_VTABLE) ShapeVTable;

    Texture = DrawingShapeVTable->GetTextureRoutine(Data, FaceHit);

    return Texture;
}

_Ret_opt_
PCNORMAL
DrawingShapeGetNormal(
    _In_ PCDRAWING_SHAPE DrawingShape,
    _In_ UINT32 FaceHit
    )
{
    PCDRAWING_SHAPE_VTABLE DrawingShapeVTable;
    PCSHAPE_VTABLE ShapeVTable;
    PCNORMAL Normal;
    PCSHAPE Shape;
    PCVOID Data;

    if (DrawingShape == NULL)
    {
        return NULL;
    }

    Shape = (PCSHAPE) DrawingShape;

    Data = ShapeGetData(Shape);

    ShapeVTable = ShapeGetVTable(Shape);

    DrawingShapeVTable = (PCDRAWING_SHAPE_VTABLE) ShapeVTable;

    Normal = DrawingShapeVTable->GetNormalRoutine(Data, FaceHit);

    return Normal;
}

VOID
DrawingShapeReference(
    _In_opt_ PDRAWING_SHAPE DrawingShape
    )
{
    PSHAPE Shape;

    Shape = (PSHAPE) DrawingShape;

    ShapeRetain(Shape);
}

VOID
DrawingShapeDereference(
    _In_opt_ _Post_invalid_ PDRAWING_SHAPE DrawingShape
    )
{
    PSHAPE Shape;

    Shape = (PSHAPE) DrawingShape;

    ShapeRelease(Shape);
}