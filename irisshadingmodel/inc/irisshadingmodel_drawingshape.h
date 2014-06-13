/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisshadingmodel_shape.h

Abstract:

    This file contains the definitions for the DRAWING_SHAPE type.

--*/

#ifndef _DRAWING_SHAPE_IRIS_SHADING_MODEL_
#define _DRAWING_SHAPE_IRIS_SHADING_MODEL_

#include <irisshadingmodel.h>

//
// Types
//

typedef
_Check_return_
_Ret_opt_
PCTEXTURE
(*PDRAWING_SHAPE_GET_TEXTURE_ROUTINE)(
    _In_opt_ PCVOID Context, 
    _In_ UINT32 FaceHit
    );

typedef
_Check_return_
_Ret_opt_
PCNORMAL 
(*PDRAWING_SHAPE_GET_NORMAL_ROUTINE)(
    _In_opt_ PCVOID Context, 
    _In_ UINT32 FaceHit
    );

typedef struct _DRAWING_SHAPE_VTABLE {
    SHAPE_VTABLE ShapeVTable;
    PDRAWING_SHAPE_GET_TEXTURE_ROUTINE GetTextureRoutine;
    PDRAWING_SHAPE_GET_NORMAL_ROUTINE GetNormalRoutine;
} DRAWING_SHAPE_VTABLE, *PDRAWING_SHAPE_VTABLE;

typedef CONST DRAWING_SHAPE_VTABLE *PCDRAWING_SHAPE_VTABLE;

//
// Functions
//

_Check_return_
_When_(DataSizeInBytes != 0 && Data != NULL && DataAlignment != 0, _Ret_opt_)
_When_(DataSizeInBytes != 0 && Data == NULL, _Ret_null_)
IRISSHADINGMODELAPI
PDRAWING_SHAPE
DrawingShapeAllocate(
    _In_ PCDRAWING_SHAPE_VTABLE DrawingShapeVTable,
    _In_reads_bytes_opt_(DataSizeInBytes) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _In_ _When_(DataSizeInBytes == 0, _Reserved_) SIZE_T DataAlignment
    );

_Ret_opt_
IRISSHADINGMODELAPI
PCTEXTURE
DrawingShapeGetTexture(
    _In_ PCDRAWING_SHAPE DrawingShape,
    _In_ UINT32 FaceHit
    );

_Ret_opt_
IRISSHADINGMODELAPI
PCNORMAL
DrawingShapeGetNormal(
    _In_ PCDRAWING_SHAPE DrawingShape,
    _In_ UINT32 FaceHit
    );

IRISSHADINGMODELAPI
VOID
DrawingShapeReference(
    _In_opt_ PDRAWING_SHAPE DrawingShape
    );

IRISSHADINGMODELAPI
VOID
DrawingShapeDereference(
    _In_opt_ _Post_invalid_ PDRAWING_SHAPE DrawingShape
    );

#endif // _DRAWING_SHAPE_IRIS_SHADING_MODEL_