/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    iriscommon_advancedsphere.h

Abstract:

    This file contains the function definitions for the advanced sphere routines.

--*/

#ifndef _ADVANCED_SPHERE_IRIS_COMMON_
#define _ADVANCED_SPHERE_IRIS_COMMON_

#include <iriscommon_sphere.h>
#include <irisadvanced.h>

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
SphereComputeNormal(
    _In_ PCSPHERE Sphere, 
    _In_ POINT3 ModelHitPoint,
    _In_ UINT32 FaceHit,
    _Out_ PVECTOR3 SurfaceNormal
    )
{
    VECTOR3 CenterToHitPoint;
    
    ASSERT(Sphere != NULL);
    ASSERT(PointValidate(ModelHitPoint) != FALSE);
    ASSERT(SurfaceNormal != NULL);

    CenterToHitPoint = PointSubtract(ModelHitPoint, Sphere->Center);
    CenterToHitPoint = VectorNormalize(CenterToHitPoint, NULL, NULL);

    if (FaceHit == SPHERE_FRONT_FACE)
    {
        *SurfaceNormal = CenterToHitPoint;
    }
    else if (FaceHit == SPHERE_BACK_FACE)
    {
        *SurfaceNormal = VectorNegate(CenterToHitPoint);
    }
    else
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS 
SphereCheckBounds(
    _In_ PCSPHERE Sphere,
    _In_ PCMATRIX ModelToWorld,
    _In_ BOUNDING_BOX WorldAlignedBoundingBox,
    _Out_ PBOOL IsInsideBox
    )
{
    ASSERT(IsInsideBox != NULL);
    
    *IsInsideBox = TRUE;
    
    return ISTATUS_SUCCESS;
}

#endif // _ADVANCED_SPHERE_IRIS_COMMON_