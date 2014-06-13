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
_When_(DataSizeInBytes != 0 && Data != NULL && DataAlignment != 0, _Ret_opt_)
_When_(DataSizeInBytes != 0 && Data == NULL, _Ret_null_)
PDRAWING_SHAPE
DrawingShapeAllocate(
    _In_ PCDRAWING_SHAPE_VTABLE DrawingShapeVTable,
    _In_reads_bytes_opt_(DataSizeInBytes) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _In_ _When_(DataSizeInBytes == 0, _Reserved_) SIZE_T DataAlignment
    )
{
    PSHAPE Shape;

    Shape = ShapeAllocate(&DrawingShapeVTable->ShapeVTable,
                          Data,
                          DataSizeInBytes,
                          DataAlignment);

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

    ShapeReference(Shape);
}

VOID
DrawingShapeDereference(
    _In_opt_ _Post_invalid_ PDRAWING_SHAPE DrawingShape
    )
{
    PSHAPE Shape;

    Shape = (PSHAPE) DrawingShape;

    ShapeDereference(Shape);
}