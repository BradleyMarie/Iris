/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    rgbshape.c

Abstract:

    This file contains the function definitions for the RGB_SHAPE type.

--*/

#include <irisrgb.h>

//
// Functions
//

_Check_return_
_Ret_opt_
PRGB_SHAPE
RgbShapeAllocate(
    _In_ PCRGB_SHAPE_VTABLE RgbShapeVTable,
    _In_reads_bytes_opt_(DataSizeInBytes) PCVOID Data,
    _In_ _When_(Data == NULL, _Reserved_) SIZE_T DataSizeInBytes,
    _In_ SIZE_T DataAlignment
    )
{
    PSHAPE Shape;

    Shape = ShapeAllocate(&RgbShapeVTable->ShapeVTable,
                          Data,
                          DataSizeInBytes,
                          DataAlignment);

    return (PRGB_SHAPE) Shape;
}

_Ret_opt_
PCTEXTURE
RgbShapeGetTexture(
    _In_ PCRGB_SHAPE RgbShape,
    _In_ UINT32 FaceHit
    )
{
    PCRGB_SHAPE_VTABLE RgbShapeVTable;
    PCSHAPE_VTABLE ShapeVTable;
    PCTEXTURE Texture;
    PCSHAPE Shape;
    PCVOID Data;

    if (RgbShape == NULL)
    {
        return NULL;
    }

    Shape = (PCSHAPE) RgbShape;

    Data = ShapeGetData(Shape);

    ShapeVTable = ShapeGetVTable(Shape);

    RgbShapeVTable = (PCRGB_SHAPE_VTABLE) ShapeVTable;

    Texture = RgbShapeVTable->GetTextureRoutine(Data, FaceHit);

    return Texture;
}

_Ret_opt_
PCNORMAL
RgbShapeGetNormal(
    _In_ PCRGB_SHAPE RgbShape,
    _In_ UINT32 FaceHit
    )
{
    PCRGB_SHAPE_VTABLE RgbShapeVTable;
    PCSHAPE_VTABLE ShapeVTable;
    PCNORMAL Normal;
    PCSHAPE Shape;
    PCVOID Data;

    if (RgbShape == NULL)
    {
        return NULL;
    }

    Shape = (PCSHAPE) RgbShape;

    Data = ShapeGetData(Shape);

    ShapeVTable = ShapeGetVTable(Shape);

    RgbShapeVTable = (PCRGB_SHAPE_VTABLE) ShapeVTable;

    Normal = RgbShapeVTable->GetNormalRoutine(Data, FaceHit);

    return Normal;
}

VOID
RgbShapeReference(
    _In_opt_ PRGB_SHAPE RgbShape
    )
{
    PSHAPE Shape;

    Shape = (PSHAPE) RgbShape;

    ShapeReference(Shape);
}

VOID
RgbShapeDereference(
    _In_opt_ _Post_invalid_ PRGB_SHAPE RgbShape
    )
{
    PSHAPE Shape;

    Shape = (PSHAPE) RgbShape;

    ShapeDereference(Shape);
}