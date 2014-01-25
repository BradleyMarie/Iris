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
    DRAWING_SHAPE ShapeHeader;
    PCSHADER Shaders[2];
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
PCSHADER 
SphereGetShader(
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

    return Sphere->Shaders[FaceHit];
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
    _In_ PCRAY Ray,
    _Inout_ PSHAPE_HIT_ALLOCATOR ShapeHitAllocator,
    _Outptr_result_maybenull_ PSHAPE_HIT_LIST *ShapeHitList
    )
{
    
    FLOAT ScalarProjectionCenterToRayOntoRay;
    FLOAT LengthSquaredCenterToOrigin;
    FLOAT LengthSquaredRayDirection;
    FLOAT LengthSquaredToChord;
    VECTOR3 CenterToRayOrigin;
    FLOAT HalfLengthOfChord;
    PCSPHERE Sphere;
    UINT32 Face0;
    UINT32 Face1;

    ASSERT(ShapeHitAllocator != NULL);
    ASSERT(ShapeHitList != NULL);
    ASSERT(Context != NULL);
    ASSERT(Ray != NULL);

    Sphere = (PCSPHERE) Context;

    VectorSubtract(&Sphere->Center, &Ray->Origin, &CenterToRayOrigin);

    ScalarProjectionCenterToRayOntoRay = VectorDotProduct(&Ray->Direction,
                                                          &CenterToRayOrigin);

    LengthSquaredCenterToOrigin = VectorDotProduct(&CenterToRayOrigin, 
                                                   &CenterToRayOrigin);

    LengthSquaredToChord = ScalarProjectionCenterToRayOntoRay * 
                           ScalarProjectionCenterToRayOntoRay -
                           LengthSquaredCenterToOrigin;

    if (LengthSquaredToChord >= Sphere->RadiusSquared)
    {
        return ISTATUS_SUCCESS;
    }

    HalfLengthOfChord = SqrtFloat(Sphere->RadiusSquared - LengthSquaredToChord);

    if (ScalarProjectionCenterToRayOntoRay > HalfLengthOfChord)
    {
        Face0 = SPHERE_FRONT_FACE;
        Face1 = SPHERE_BACK_FACE;
    }
    else if (ScalarProjectionCenterToRayOntoRay < (FLOAT) 0.0)
    {
        Face0 = SPHERE_BACK_FACE;
        Face1 = SPHERE_FRONT_FACE;
    }
    else
    {
        Face0 = SPHERE_BACK_FACE;
        Face1 = SPHERE_BACK_FACE;
    }

    *ShapeHitList = ShapeHitAllocatorAllocate(ShapeHitAllocator,
                                              NULL,
                                              (PCSHAPE) Context,
                                              ScalarProjectionCenterToRayOntoRay - HalfLengthOfChord,
                                              Face0,
                                              NULL,
                                              0);

    if (*ShapeHitList == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    *ShapeHitList = ShapeHitAllocatorAllocate(ShapeHitAllocator,
                                              *ShapeHitList,
                                              (PCSHAPE) Context,
                                              ScalarProjectionCenterToRayOntoRay + HalfLengthOfChord,
                                              Face1,
                                              NULL,
                                              0);

    return (*ShapeHitList == NULL) ? ISTATUS_ALLOCATION_FAILED : ISTATUS_SUCCESS;
}

//
// Static variables
//

STATIC DRAWING_SHAPE_VTABLE SphereHeader = {
    { SphereTraceSphere, free },
    SphereGetShader,
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
    _In_opt_ PCSHADER FrontShader,
    _In_opt_ PCNORMAL FrontNormal,
    _In_opt_ PCSHADER BackShader,
    _In_opt_ PCNORMAL BackNormal
    )
{
    PSPHERE Sphere;

    ASSERT(Center != NULL);

    if (Radius == (FLOAT) 0.0 ||
        !IsNormalFloat(Radius) ||
        !IsFiniteFloat(Radius))
    {
        return NULL;
    }

    Sphere = (PSPHERE) malloc(sizeof(SPHERE));

    if (Sphere == NULL)
    {
        return NULL;
    }

    Sphere->ShapeHeader.DrawingShapeVTable = &SphereHeader;

    Sphere->Center = *Center;
    Sphere->RadiusSquared = Radius * Radius;

    Sphere->Shaders[0] = FrontShader;
    Sphere->Shaders[1] = BackShader;
    Sphere->Normals[0] = FrontNormal;
    Sphere->Normals[1] = BackNormal;

    return (PDRAWING_SHAPE) Sphere;
}