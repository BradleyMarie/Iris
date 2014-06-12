/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    iristoolkit_sphere.h

Abstract:

    This file contains the definitions for the SPHERE type.

--*/

#include <iristoolkitp.h>

//
// Constants
//

#define SPHERE_FRONT_FACE 0
#define SPHERE_BACK_FACE  1

//
// Types
//

typedef struct _SPHERE {
    PCTEXTURE Textures[2];
    PCNORMAL Normals[2];
    POINT3 Center;
    FLOAT RadiusSquared;
} SPHERE, *PSPHERE;

typedef CONST SPHERE *PCSPHERE;

//
// Static functions
//

_Check_return_
_Ret_maybenull_
STATIC
PCTEXTURE 
SphereGetTexture(
    _In_ PCVOID Context, 
    _In_ UINT32 FaceHit
    )
{
    PCSPHERE Sphere;

    ASSERT(Context != NULL);

    Sphere = (PCSPHERE) Context;

    if (FaceHit > SPHERE_BACK_FACE)
    {
        return NULL;
    }

    return Sphere->Textures[FaceHit];
}

_Check_return_
_Ret_maybenull_
STATIC
PCNORMAL 
SphereGetNormal(
    _In_ PCVOID Context, 
    _In_ UINT32 FaceHit
    )
{
    PCSPHERE Sphere;

    ASSERT(Context != NULL);

    Sphere = (PCSPHERE) Context;

    if (FaceHit > SPHERE_BACK_FACE)
    {
        return NULL;
    }

    return Sphere->Normals[FaceHit];
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS 
SphereTraceSphere(
    _In_ PCVOID Context, 
    _In_ RAY Ray,
    _Inout_ PSHAPE_HIT_ALLOCATOR ShapeHitAllocator,
    _Outptr_result_maybenull_ PSHAPE_HIT_LIST *ShapeHitList
    )
{
    FLOAT NegatedScalarProjectionOriginToCenterOntoRay;
    FLOAT ScalarProjectionOriginToCenterOntoRay;
    FLOAT LengthSquaredCenterToOrigin;
    VECTOR3 CenterToRayOrigin;
    FLOAT LengthOfRaySquared;
    FLOAT Discriminant;
    FLOAT Distance0;
    PCSPHERE Sphere;
    UINT32 Face0;

#if defined(ENABLE_CSG_SUPPORT)

    UINT32 Face1;
    FLOAT Distance1;

#endif // defined(ENABLE_CSG_SUPPORT)

    ASSERT(ShapeHitAllocator != NULL);
    ASSERT(ShapeHitList != NULL);
    ASSERT(Context != NULL);

    Sphere = (PCSPHERE) Context;

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

    *ShapeHitList = ShapeHitAllocatorAllocate(ShapeHitAllocator,
                                              NULL,
                                              Distance0,
                                              Face0,
                                              NULL,
                                              0);

    if (*ShapeHitList == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

#if defined(ENABLE_CSG_SUPPORT)

    Distance1 = (NegatedScalarProjectionOriginToCenterOntoRay + Discriminant) /
                LengthOfRaySquared;

    *ShapeHitList = ShapeHitAllocatorAllocate(ShapeHitAllocator,
                                              *ShapeHitList,
                                              Distance1,
                                              Face1,
                                              NULL,
                                              0);

    if (*ShapeHitList == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

#endif // defined(ENABLE_CSG_SUPPORT)

    return ISTATUS_SUCCESS;
}

//
// Static variables
//

CONST STATIC DRAWING_SHAPE_VTABLE SphereHeader = {
    { SphereTraceSphere, NULL },
    SphereGetTexture,
    SphereGetNormal
};

//
// Public Functions
//

_Check_return_
_Ret_maybenull_
IRISTOOLKITAPI
PDRAWING_SHAPE
SphereAllocate(
    _In_ PCPOINT3 Center,
    _In_ FLOAT Radius,
    _In_opt_ PCTEXTURE FrontTexture,
    _In_opt_ PCNORMAL FrontNormal,
    _In_opt_ PCTEXTURE BackTexture,
    _In_opt_ PCNORMAL BackNormal
    )
{
    PDRAWING_SHAPE DrawingShape;
    SPHERE Sphere;

    ASSERT(Center != NULL);

    if (Radius == (FLOAT) 0.0 ||
        IsNormalFloat(Radius) == FALSE ||
        IsFiniteFloat(Radius) == FALSE)
    {
        return NULL;
    }

    if (FrontTexture == NULL &&
        BackTexture == NULL)
    {
        return NULL;
    }

    Sphere.Center = *Center;
    Sphere.RadiusSquared = Radius * Radius;
    Sphere.Textures[0] = FrontTexture;
    Sphere.Textures[1] = BackTexture;
    Sphere.Normals[0] = FrontNormal;
    Sphere.Normals[1] = BackNormal;

    DrawingShape = DrawingShapeAllocate(&SphereHeader,
                                        &Sphere,
                                        sizeof(SPHERE),
                                        sizeof(PVOID));

    return DrawingShape;
}