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
_Ret_maybenull_
PCSHADER 
(*PDRAWING_SHAPE_GET_SHADER_ROUTINE)(
    _In_ PCVOID Context, 
    _In_ UINT32 FaceHit
    );

typedef
_Check_return_
_Ret_maybenull_
PCNORMAL 
(*PDRAWING_SHAPE_GET_NORMAL_ROUTINE)(
    _In_ PCVOID Context, 
    _In_ UINT32 FaceHit
    );

typedef struct _DRAWING_SHAPE_VTABLE {
    SHAPE_VTABLE ShapeVTable;
    PFREE_ROUTINE FreeRoutine;
    PDRAWING_SHAPE_GET_SHADER_ROUTINE GetShaderRoutine;
    PDRAWING_SHAPE_GET_NORMAL_ROUTINE GetNormalRoutine;
} DRAWING_SHAPE_VTABLE, *PDRAWING_SHAPE_VTABLE;

typedef CONST DRAWING_SHAPE_VTABLE *PCDRAWING_SHAPE_VTABLE;

struct _DRAWING_SHAPE {
    PCDRAWING_SHAPE_VTABLE DrawingShapeVTable;
};

//
// Functions
//

SFORCEINLINE
VOID
DrawingShapeFree(
    _Pre_maybenull_ _Post_invalid_ PDRAWING_SHAPE Shape
    )
{
    if (Shape == NULL)
    {
        return;
    }

    Shape->DrawingShapeVTable->FreeRoutine(Shape);
}

#endif // _DRAWING_SHAPE_IRIS_SHADING_MODEL_