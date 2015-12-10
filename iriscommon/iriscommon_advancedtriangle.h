/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    iriscommon_advancedtriangle.h

Abstract:

    This file contains the function definitions for the advanced triangle routines.

--*/

#ifndef _ADVANCED_TRIANGLE_IRIS_COMMON_
#define _ADVANCED_TRIANGLE_IRIS_COMMON_

#include <iriscommon_triangle.h>
#include <irisadvanced.h>

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
TriangleComputeNormal(
    _In_ PCTRIANGLE Triangle, 
    _In_ POINT3 ModelHitPoint,
    _In_ UINT32 FaceHit,
    _Out_ PVECTOR3 SurfaceNormal
    )
{
    ASSERT(Triangle != NULL);
    ASSERT(PointValidate(ModelHitPoint) != FALSE);
    ASSERT(FaceHit == TRIANGLE_FRONT_FACE || FaceHit == TRIANGLE_BACK_FACE);
    ASSERT(SurfaceNormal != NULL);

    if (FaceHit == TRIANGLE_FRONT_FACE)
    {
        *SurfaceNormal = Triangle->SurfaceNormal;
    }
    else
    {
        *SurfaceNormal = VectorNegate(Triangle->SurfaceNormal);
    }

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS 
TriangleCheckBounds(
    _In_ PCTRIANGLE Triangle,
    _In_ PCMATRIX ModelToWorld,
    _In_ BOUNDING_BOX WorldAlignedBoundingBox,
    _Out_ PBOOL IsInsideBox
    )
{
    POINT3 TransformedVertex0;
    POINT3 TransformedVertex1;
    POINT3 TransformedVertex2;

    ASSERT(Triangle != NULL);
    ASSERT(IsInsideBox != NULL);

    TransformedVertex0 = PointMatrixMultiply(ModelToWorld, Triangle->Vertex0);

    if (BoundingBoxContainsPoint(WorldAlignedBoundingBox, TransformedVertex0) != FALSE)
    {
        *IsInsideBox = TRUE;
        return ISTATUS_SUCCESS;
    }
    
    TransformedVertex1 = PointVectorAdd(Triangle->Vertex0, Triangle->B);
    TransformedVertex1 = PointMatrixMultiply(ModelToWorld, TransformedVertex1);

    if (BoundingBoxContainsPoint(WorldAlignedBoundingBox, TransformedVertex1) != FALSE)
    {
        *IsInsideBox = TRUE;
        return ISTATUS_SUCCESS;
    }

    TransformedVertex2 = PointVectorAdd(Triangle->Vertex0, Triangle->C);
    TransformedVertex2 = PointMatrixMultiply(ModelToWorld, TransformedVertex2);

    if (BoundingBoxContainsPoint(WorldAlignedBoundingBox, TransformedVertex2) != FALSE)
    {
        *IsInsideBox = TRUE;
        return ISTATUS_SUCCESS;
    }

    *IsInsideBox = FALSE;
    return ISTATUS_SUCCESS;
}

#endif // _ADVANCED_TRIANGLE_IRIS_COMMON_