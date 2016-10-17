/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    sphere.c

Abstract:

    This file contains the definitions for sphere types.

--*/

#include <irisphysxtoolkitp.h>

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
SphereTestRay(
    _In_opt_ PCSPHERE Sphere,
    _In_ RAY Ray,
    _Inout_ PPBR_HIT_ALLOCATOR HitAllocator,
    _Outptr_result_maybenull_ PHIT_LIST *HitList
    )
{
    UINT32 BackFace0;
    VECTOR3 CenterToRayOrigin;
    FLOAT Discriminant;
    FLOAT Distance0;
    UINT32 FrontFace0;
    FLOAT LengthOfRaySquared;
    FLOAT LengthSquaredCenterToOrigin;
    FLOAT NegatedScalarProjectionOriginToCenterOntoRay;
    FLOAT ScalarProjectionOriginToCenterOntoRay;
    ISTATUS Status;

#if defined(ENABLE_CSG_SUPPORT)
    
    UINT32 BackFace1;
    UINT32 FrontFace1;
    FLOAT Distance1;

#endif // defined(ENABLE_CSG_SUPPORT)

    ASSERT(Sphere != NULL);
    ASSERT(RayValidate(Ray) != FALSE);
    ASSERT(HitAllocator != NULL);
    ASSERT(HitList != NULL);

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

#ifndef ENABLE_CSG_SUPPORT
    if (Distance0 < (FLOAT) 0.0)
    {
        //
        // Intersection is behind ray origin
        //

        *HitList = NULL;
        return ISTATUS_SUCCESS;
    }
#endif // ENABLE_CSG_SUPPORT

    if (LengthSquaredCenterToOrigin < Sphere->RadiusSquared)
    {
        FrontFace0 = SPHERE_BACK_FACE;
        BackFace0 = SPHERE_FRONT_FACE;
#if defined(ENABLE_CSG_SUPPORT)
        FrontFace1 = SPHERE_BACK_FACE;
        BackFace1 = SPHERE_FRONT_FACE;
#endif // defined(ENABLE_CSG_SUPPORT)
    }
    else if (Distance0 >(FLOAT) 0.0)
    {
        FrontFace0 = SPHERE_BACK_FACE;
        BackFace0 = SPHERE_FRONT_FACE;
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

    Status = PBRHitAllocatorAllocate(HitAllocator,
                                     NULL,
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

    Status = PBRHitAllocatorAllocate(HitAllocator,
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

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
SphereComputeNormal(
    _In_ PCSPHERE Sphere,
    _In_ POINT3 HitPoint,
    _In_ UINT32 FaceHit,
    _Out_ PVECTOR3 SurfaceNormal
    )
{
    VECTOR3 CenterToHitPoint;

    ASSERT(Sphere != NULL);
    ASSERT(PointValidate(HitPoint) != FALSE);
    ASSERT(SurfaceNormal != NULL);

    CenterToHitPoint = PointSubtract(HitPoint, Sphere->Center);
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

//
// Types
//

typedef struct _PHYSX_SPHERE {
    PPBR_MATERIAL Materials[2];
    SPHERE Data;
} PHYSX_SPHERE, *PPHYSX_SPHERE;

typedef CONST PHYSX_SPHERE *PCPHYSX_SPHERE;

typedef struct _PHYSX_LIGHT_SPHERE {
    PPBR_MATERIAL Materials[2];
    PPBR_LIGHT Lights[2];
    SPHERE Data;
} PHYSX_LIGHT_SPHERE, *PPHYSX_LIGHT_SPHERE;

typedef CONST PHYSX_LIGHT_SPHERE *PCPHYSX_LIGHT_SPHERE;

//
// Static Functions
//

_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS 
PhysxSphereGetMaterial(
    _In_ PCVOID Context, 
    _In_ UINT32 FaceHit,
    _Out_opt_ PCPBR_MATERIAL *Material
    )
{
    PCPHYSX_SPHERE Sphere;

    ASSERT(Context != NULL);
    ASSERT(Material != NULL);

    Sphere = (PCPHYSX_SPHERE) Context;

    if (FaceHit > SPHERE_BACK_FACE)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }
    
    *Material = Sphere->Materials[FaceHit];

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS
PhysxSphereComputeNormal(
    _In_ PCVOID Context, 
    _In_ POINT3 ModelHitPoint,
    _In_ UINT32 FaceHit,
    _Out_ PVECTOR3 SurfaceNormal
    )
{
    PCPHYSX_SPHERE Sphere;
    ISTATUS Status;

    ASSERT(Context != NULL);
    ASSERT(SurfaceNormal != NULL);

    Sphere = (PCPHYSX_SPHERE) Context;
    
    Status = SphereComputeNormal(&Sphere->Data,
                                 ModelHitPoint,
                                 FaceHit,
                                 SurfaceNormal);
                                   
    return Status;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS 
PhysxSphereTestBounds(
    _In_ PCVOID Context,
    _In_ PCMATRIX ModelToWorld,
    _In_ BOUNDING_BOX WorldAlignedBoundingBox,
    _Out_ PBOOL IsInsideBox
    )
{
    PCPHYSX_SPHERE Sphere;
    ISTATUS Status;

    ASSERT(Context != NULL);
    ASSERT(IsInsideBox != NULL);

    Sphere = (PCPHYSX_SPHERE) Context;
    
    Status = SphereCheckBounds(&Sphere->Data,
                               ModelToWorld,
                               WorldAlignedBoundingBox,
                               IsInsideBox);
                                 
    return Status;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS 
PhysxSphereTestRay(
    _In_opt_ PCVOID Context, 
    _In_ RAY Ray,
    _Inout_ PPBR_HIT_ALLOCATOR HitAllocator,
    _Outptr_result_maybenull_ PHIT_LIST *HitList
    )
{
    PCPHYSX_SPHERE Sphere;
    ISTATUS Status;

    ASSERT(Context != NULL);
    ASSERT(RayValidate(Ray) != FALSE);
    ASSERT(HitAllocator != NULL);
    ASSERT(HitList != NULL);

    Sphere = (PCPHYSX_SPHERE) Context;

    Status = SphereTestRay(&Sphere->Data,
                           Ray,
                           HitAllocator,
                           HitList);

    return Status;
}

STATIC
VOID
PhysxSphereFree(
    _In_ _Post_invalid_ PVOID Context
    )
{
    PCPHYSX_SPHERE Sphere;

    ASSERT(Context != NULL);

    Sphere = (PCPHYSX_SPHERE) Context;

    PbrMaterialRelease(Sphere->Materials[SPHERE_FRONT_FACE]);
    PbrMaterialRelease(Sphere->Materials[SPHERE_BACK_FACE]);
}

//
// Static Variables
//

CONST STATIC PBR_GEOMETRY_VTABLE SphereHeader = {
    PhysxSphereTestRay,
    PhysxSphereComputeNormal,
    PhysxSphereTestBounds,
    PhysxSphereGetMaterial,
    PhysxSphereFree
};

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PhysxSphereAllocate(
    _In_ POINT3 Center,
    _In_ FLOAT Radius,
    _In_opt_ PPBR_MATERIAL FrontMaterial,
    _In_opt_ PPBR_MATERIAL BackMaterial,
    _Out_ PPBR_GEOMETRY *Geometry
    )
{
    PCPBR_GEOMETRY_VTABLE GeometryVTable;
    PHYSX_SPHERE Sphere;
    SIZE_T DataAlignment;
    FLOAT RadiusSquared;
    SIZE_T DataSize;
    ISTATUS Status;
    PCVOID Data;

    if (PointValidate(Center) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }
    
    if (IsFiniteFloat(Radius) == FALSE ||
        IsGreaterThanZeroFloat(Radius) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }
    
    RadiusSquared = Radius * Radius;
    
    if (IsFiniteFloat(Radius) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }
    
    if (Geometry == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_06;
    }

	SphereInitialize(&Sphere.Data,
					 Center,
					 RadiusSquared);

	Sphere.Materials[SPHERE_FRONT_FACE] = FrontMaterial;
	Sphere.Materials[SPHERE_BACK_FACE] = BackMaterial;

	Data = &Sphere;
	DataSize = sizeof(PHYSX_SPHERE);
	DataAlignment = _Alignof(PHYSX_SPHERE);
	GeometryVTable = &SphereHeader;
    
    Status = PBRGeometryAllocate(GeometryVTable,
                                 Data,
                                 DataSize,
                                 DataAlignment,
                                 Geometry);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    PbrMaterialRetain(FrontMaterial);

    return ISTATUS_SUCCESS;
}
