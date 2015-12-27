/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    iriscommon_sphere.h

Abstract:

    This file contains the function definitions for the sphere routines.

--*/

#ifndef _SPHERE_IRIS_COMMON_
#define _SPHERE_IRIS_COMMON_

#include <iris.h>

//
// Constants
//

#define SPHERE_FRONT_FACE 0
#define SPHERE_BACK_FACE  1

//
// Types
//

typedef struct _SPHERE {
    POINT3 Center;
    FLOAT RadiusSquared;
} SPHERE, *PSPHERE;

typedef CONST SPHERE *PCSPHERE;

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS 
SphereTrace(
    _In_opt_ PCSPHERE Sphere, 
    _In_ RAY Ray,
    _Inout_ PSHAPE_HIT_ALLOCATOR ShapeHitAllocator,
    _Outptr_result_maybenull_ PSHAPE_HIT_LIST *ShapeHitList
    )
{
    VECTOR3 CenterToRayOrigin;
    FLOAT Discriminant;
    FLOAT Distance0;
    UINT32 Face0;
    FLOAT LengthOfRaySquared;
    FLOAT LengthSquaredCenterToOrigin;
    FLOAT NegatedScalarProjectionOriginToCenterOntoRay;
    FLOAT ScalarProjectionOriginToCenterOntoRay;
    ISTATUS Status;

#if defined(ENABLE_CSG_SUPPORT)

    UINT32 Face1;
    FLOAT Distance1;

#endif // defined(ENABLE_CSG_SUPPORT)

    ASSERT(Sphere != NULL);
    ASSERT(RayValidate(Ray) != FALSE);
    ASSERT(ShapeHitAllocator != NULL);
    ASSERT(ShapeHitList != NULL);

    CenterToRayOrigin = PointSubtract(Ray.Origin, Sphere->Center);

    ScalarProjectionOriginToCenterOntoRay = VectorDotProduct(Ray.Direction,
                                                             CenterToRayOrigin);

    LengthOfRaySquared = VectorDotProduct(Ray.Direction, Ray.Direction);

    LengthSquaredCenterToOrigin = VectorDotProduct(CenterToRayOrigin, 
                                                   CenterToRayOrigin);

    Discriminant = ScalarProjectionOriginToCenterOntoRay * 
                   ScalarProjectionOriginToCenterOntoRay -
                   LengthOfRaySquared *
                   (LengthSquaredCenterToOrigin - Sphere->RadiusSquared);

    if (Discriminant < (FLOAT) 0.0)
    {
        //
        // Misses sphere completely
        //

        *ShapeHitList = NULL;
        return ISTATUS_SUCCESS;
    }

    if (IsZeroFloat(Discriminant) != FALSE)
    {
        //
        // Hits One Point
        //

        *ShapeHitList = NULL;
        return ISTATUS_SUCCESS;
    }

    //
    // Hits two points
    //

    NegatedScalarProjectionOriginToCenterOntoRay = -ScalarProjectionOriginToCenterOntoRay;

    Discriminant = SqrtFloat(Discriminant);

    Distance0 = (NegatedScalarProjectionOriginToCenterOntoRay - Discriminant) /
                LengthOfRaySquared;

    if (LengthSquaredCenterToOrigin < Sphere->RadiusSquared)
    {
        Face0 = SPHERE_BACK_FACE;
#if defined(ENABLE_CSG_SUPPORT)
        Face1 = SPHERE_BACK_FACE;
#endif // defined(ENABLE_CSG_SUPPORT)
    }
    else if (Distance0 > (FLOAT) 0.0)
    {
        Face0 = SPHERE_FRONT_FACE;
#if defined(ENABLE_CSG_SUPPORT)
        Face1 = SPHERE_BACK_FACE;
#endif // defined(ENABLE_CSG_SUPPORT)
    }
    else
    {
        Face0 = SPHERE_BACK_FACE;
#if defined(ENABLE_CSG_SUPPORT)
        Face1 = SPHERE_FRONT_FACE;
#endif // defined(ENABLE_CSG_SUPPORT)
    }

    Status = ShapeHitAllocatorAllocate(ShapeHitAllocator,
                                       NULL,
                                       Distance0,
                                       Face0,
                                       NULL,
                                       0,
                                       0,
                                       ShapeHitList);

#if defined(ENABLE_CSG_SUPPORT)

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    Distance1 = (NegatedScalarProjectionOriginToCenterOntoRay + Discriminant) /
                LengthOfRaySquared;

    Status = ShapeHitAllocatorAllocate(ShapeHitAllocator,
                                       *ShapeHitList,
                                       Distance1,
                                       Face1,
                                       NULL,
                                       0,
                                       0,
                                       ShapeHitList);

#endif // defined(ENABLE_CSG_SUPPORT)

    return Status;
}

VOID
SphereInitialize(
    _Out_ PSPHERE Sphere,
    _In_ POINT3 Center,
    _In_ FLOAT RadiusSquared
    )
{   
    ASSERT(PointValidate(Center) != FALSE);
    ASSERT(IsFiniteFloat(RadiusSquared) != FALSE);
    ASSERT(IsGreaterThanZeroFloat(RadiusSquared) != FALSE);
    
    Sphere->Center = Center;
    Sphere->RadiusSquared = RadiusSquared;
}

#endif // _TRIANGLE_IRIS_COMMON_