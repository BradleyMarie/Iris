/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisshadingmodel_drawingshape.h

Abstract:

    This file contains the internal definitions for the DRAWING_SHAPE type.

--*/

#ifndef _DRAWING_SHAPE_IRIS_SHADING_MODEL_INTERNAL_
#define _DRAWING_SHAPE_IRIS_SHADING_MODEL_INTERNAL_

#include <irisshadingmodelp.h>

//
// Functions
//

SFORCEINLINE
PCSHADER
DrawingShapeGetShader(
    _In_ PCDRAWING_SHAPE Shape,
    _In_ UINT32 FaceHit
    )
{
    PCSHADER Shader;

    ASSERT(Shape != NULL);

    Shader = Shape->DrawingShapeVTable->GetShaderRoutine(Shape, FaceHit);
    
    return Shader;
}

SFORCEINLINE
PCNORMAL
DrawingShapeGetNormal(
    _In_ PCDRAWING_SHAPE Shape,
    _In_ UINT32 FaceHit
    )
{
    PCNORMAL Normal;

    ASSERT(Shape != NULL);

    Normal = Shape->DrawingShapeVTable->GetNormalRoutine(Shape, FaceHit);
    
    return Normal;
}

#endif // _DRAWING_SHAPE_IRIS_SHADING_MODEL_INTERNAL_