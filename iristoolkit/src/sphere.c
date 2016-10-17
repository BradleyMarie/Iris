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
    PTEXTURE Textures[2];
    PNORMAL Normals[2];
    POINT3 Center;
    FLOAT RadiusSquared;
} SPHERE, *PSPHERE;

typedef CONST SPHERE *PCSPHERE;

//
// Static functions
//

_Check_return_
_Ret_opt_
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
_Ret_opt_
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
SphereTestRay(
    _In_ PCDRAWING_SHAPE DrawingShape,
    _In_opt_ PCVOID Context, 
    _In_ RAY Ray,
    _Inout_ PHIT_ALLOCATOR HitAllocator,
    _Outptr_result_maybenull_ PHIT_LIST *HitList
    )
{
    VECTOR3 CenterToRayOrigin;
    FLOAT Discriminant;
    FLOAT Distance0;
    UINT32 FrontFace0;
    UINT32 BackFace0;
    FLOAT LengthOfRaySquared;
    FLOAT LengthSquaredCenterToOrigin;
    FLOAT NegatedScalarProjectionOriginToCenterOntoRay;
    FLOAT ScalarProjectionOriginToCenterOntoRay;
    PCSPHERE Sphere;
    ISTATUS Status;

#if defined(ENABLE_CSG_SUPPORT)

    UINT32 Face1;
    FLOAT Distance1;

#endif // defined(ENABLE_CSG_SUPPORT)

    ASSERT(HitAllocator != NULL);
    ASSERT(HitList != NULL);
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

        *HitList = NULL;
        return ISTATUS_SUCCESS;
    }

    if (IsZeroFloat(Discriminant) != FALSE)
    {
        //
        // Hits One Point
        //

        *HitList = NULL;
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
        FrontFace0 = SPHERE_BACK_FACE;
        BackFace0 = SPHERE_FRONT_FACE;
#if defined(ENABLE_CSG_SUPPORT)
        FrontFace1 = SPHERE_BACK_FACE;
        BackFace1 = SPHERE_FRONT_FACE;
#endif // defined(ENABLE_CSG_SUPPORT)
    }
    else if (Distance0 > (FLOAT) 0.0)
    {
        FrontFace0 = SPHERE_FRONT_FACE;
        BackFace0 =SPHERE_BACK_FACE;
#if defined(ENABLE_CSG_SUPPORT)
        FrontFace1 = SPHERE_BACK_FACE;
        BackFace1 = SPHERE_FRONT_FACE;
#endif // defined(ENABLE_CSG_SUPPORT)
    }
    else
    {
        FrontFace0 = SPHERE_BACK_FACE;
        BackFace0 = SPHERE_FRONT_FACE;
#if defined(ENABLE_CSG_SUPPORT)
        FrontFace1 = SPHERE_FRONT_FACE;
        BackFace1 = SPHERE_BACK_FACE;
#endif // defined(ENABLE_CSG_SUPPORT)
    }

    Status = HitAllocatorAllocate(HitAllocator,
                                  NULL,
                                  DrawingShape,
                                  Distance0,
                                  FrontFace0,
                                  BackFace0,
                                  NULL,
                                  0,
                                  0,
                                  HitList);

#if defined(ENABLE_CSG_SUPPORT)

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    Distance1 = (NegatedScalarProjectionOriginToCenterOntoRay + Discriminant) /
                LengthOfRaySquared;

    Status = HitAllocatorAllocate(HitAllocator,
                                  *HitList,
                                  Distance1,
                                  FrontFace1,
                                  BackFace1,
                                  NULL,
                                  0,
                                  0,
                                  HitList);

#endif // defined(ENABLE_CSG_SUPPORT)

    return Status;
}

STATIC
VOID
SphereFree(
    _In_ _Post_invalid_ PVOID Context
    )
{
    PSPHERE Sphere;

    ASSERT(Context != NULL);

    Sphere = (PSPHERE) Context;

    TextureDereference(Sphere->Textures[SPHERE_FRONT_FACE]);
    TextureDereference(Sphere->Textures[SPHERE_BACK_FACE]);
    NormalDereference(Sphere->Normals[SPHERE_FRONT_FACE]);
    NormalDereference(Sphere->Normals[SPHERE_BACK_FACE]);
}

//
// Static variables
//

CONST STATIC DRAWING_SHAPE_VTABLE SphereHeader = {
    SphereTestRay, 
    SphereFree,
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
    _In_ POINT3 Center,
    _In_ FLOAT Radius,
    _In_opt_ PTEXTURE FrontTexture,
    _In_opt_ PNORMAL FrontNormal,
    _In_opt_ PTEXTURE BackTexture,
    _In_opt_ PNORMAL BackNormal
    )
{
    PDRAWING_SHAPE DrawingShape;
    SPHERE Sphere;

    if (PointValidate(Center) == FALSE ||
        Radius == (FLOAT) 0.0 ||
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

    Sphere.Center = Center;
    Sphere.RadiusSquared = Radius * Radius;
    Sphere.Textures[SPHERE_FRONT_FACE] = FrontTexture;
    Sphere.Textures[SPHERE_BACK_FACE] = BackTexture;
    Sphere.Normals[SPHERE_FRONT_FACE] = FrontNormal;
    Sphere.Normals[SPHERE_BACK_FACE] = BackNormal;

    DrawingShape = DrawingShapeAllocate(&SphereHeader,
                                        &Sphere,
                                        sizeof(SPHERE),
                                        sizeof(PVOID));

    if (DrawingShape != NULL)
    {
        TextureReference(FrontTexture);
        TextureReference(BackTexture);
        NormalReference(FrontNormal);
        NormalReference(BackNormal);
    }

    return DrawingShape;
}
