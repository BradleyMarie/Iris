/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisrgb_rgbshape.h

Abstract:

    This file contains the definitions for the RGB_SHAPE type.

--*/

#ifndef _DRAWING_SHAPE_IRIS_RGB_
#define _DRAWING_SHAPE_IRIS_RGB_

#include <irisrgb.h>

//
// Types
//

typedef
_Check_return_
_Ret_opt_
PCTEXTURE
(*PRGB_SHAPE_GET_TEXTURE_ROUTINE)(
    _In_opt_ PCVOID Context, 
    _In_ UINT32 FaceHit
    );

typedef
_Check_return_
_Ret_opt_
PCNORMAL 
(*PRGB_SHAPE_GET_NORMAL_ROUTINE)(
    _In_opt_ PCVOID Context, 
    _In_ UINT32 FaceHit
    );

typedef struct _RGB_SHAPE_VTABLE {
    ADVANCED_SHAPE_VTABLE AdvancedShapeVTable;
    PRGB_SHAPE_GET_TEXTURE_ROUTINE GetTextureRoutine;
    PRGB_SHAPE_GET_NORMAL_ROUTINE GetNormalRoutine;
} RGB_SHAPE_VTABLE, *PRGB_SHAPE_VTABLE;

typedef CONST RGB_SHAPE_VTABLE *PCRGB_SHAPE_VTABLE;

typedef struct _RGB_SHAPE RGB_SHAPE, *PRGB_SHAPE;
typedef CONST RGB_SHAPE *PCRGB_SHAPE;

//
// Functions
//

_Check_return_
_Ret_opt_
IRISRGBAPI
PRGB_SHAPE
RgbShapeAllocate(
    _In_ PCRGB_SHAPE_VTABLE RgbShapeVTable,
    _In_reads_bytes_opt_(DataSizeInBytes) PCVOID Data,
    _In_ _When_(Data == NULL, _Reserved_) SIZE_T DataSizeInBytes,
    _In_ SIZE_T DataAlignment
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISRGBAPI
ISTATUS 
RgbShapeCheckBounds(
    _In_ PCRGB_SHAPE RgbShape,
    _In_ PCMATRIX ModelToWorld,
    _In_ BOUNDING_BOX WorldAlignedBoundingBox,
    _Out_ PBOOL IsInsideBox
    );

_Ret_opt_
IRISRGBAPI
PCTEXTURE
RgbShapeGetTexture(
    _In_ PCRGB_SHAPE RgbShape,
    _In_ UINT32 FaceHit
    );

_Ret_opt_
IRISRGBAPI
PCNORMAL
RgbShapeGetNormal(
    _In_ PCRGB_SHAPE RgbShape,
    _In_ UINT32 FaceHit
    );

IRISRGBAPI
VOID
RgbShapeReference(
    _In_opt_ PRGB_SHAPE RgbShape
    );

IRISRGBAPI
VOID
RgbShapeDereference(
    _In_opt_ _Post_invalid_ PRGB_SHAPE RgbShape
    );

#endif // _DRAWING_SHAPE_IRIS_RGB_