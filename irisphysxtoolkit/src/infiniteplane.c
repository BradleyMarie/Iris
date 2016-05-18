/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    infiniteplane.c

Abstract:

    This file contains the definitions for Infinite Plane types.

--*/

#include <irisphysxtoolkitp.h>

//
// Constants
//

#define INFINITE_PLANE_FRONT_FACE 0
#define INFINITE_PLANE_BACK_FACE  1

//
// Types
//

typedef struct _INFINITE_PLANE {
    POINT3 Point;
    VECTOR3 SurfaceNormal;
} INFINITE_PLANE, *PINFINITE_PLANE;

typedef CONST INFINITE_PLANE *PCINFINITE_PLANE;

//
// Infinite Plane Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
InfinitePlaneTestRay(
    _In_ PCINFINITE_PLANE Plane,
    _In_ RAY Ray,
    _Inout_ PPBR_HIT_ALLOCATOR HitAllocator,
    _Outptr_result_maybenull_ PHIT_LIST *HitList
    )
{
    FLOAT Distance;
    FLOAT DotProduct;
    INT32 Face;
    ISTATUS Status;
    VECTOR3 Temp;

    ASSERT(Plane != NULL);
    ASSERT(RayValidate(Ray) != FALSE);
    ASSERT(HitAllocator != NULL);
    ASSERT(HitList != NULL);

    Temp = PointSubtract(Ray.Origin, Plane->Point);

    DotProduct = VectorDotProduct(Ray.Direction, Plane->SurfaceNormal);
    Distance = VectorDotProduct(Temp, Plane->SurfaceNormal) / -DotProduct;

    if (IsFiniteFloat(Distance) == FALSE)
    {
        *HitList = NULL;
        return ISTATUS_SUCCESS;
    }

#if !defined(ENABLE_CSG_SUPPORT)

    if (IsGreaterThanOrEqualToZeroFloat(Distance) == FALSE)
    {
        *HitList = NULL;
        return ISTATUS_SUCCESS;
    }

#endif // !defined(ENABLE_CSG_SUPPORT)

    Face = ((FLOAT) 0.0 > DotProduct) ? INFINITE_PLANE_FRONT_FACE : INFINITE_PLANE_BACK_FACE;

    Status = PBRHitAllocatorAllocate(HitAllocator,
                                     NULL,
                                     Distance,
                                     Face,
                                     NULL,
                                     0,
                                     0,
                                     HitList);

    return Status;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
InfinitePlaneInitialize(
    _In_ POINT3 Point,
    _In_ VECTOR3 SurfaceNormal,
    _Out_ PINFINITE_PLANE Plane
    )
{
    ASSERT(Plane != NULL);

    if (PointValidate(Point) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (VectorValidate(SurfaceNormal) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    Plane->Point = Point;
    Plane->SurfaceNormal = VectorNormalize(SurfaceNormal, NULL, NULL);

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
InfinitePlaneComputeNormal(
    _In_ PCINFINITE_PLANE Plane,
    _In_ POINT3 ModelHitPoint,
    _In_ UINT32 FaceHit,
    _Out_ PVECTOR3 SurfaceNormal
    )
{
    ASSERT(Plane != NULL);
    ASSERT(SurfaceNormal != NULL);

    if (FaceHit == INFINITE_PLANE_FRONT_FACE)
    {
        *SurfaceNormal = Plane->SurfaceNormal;
    }
    else if (FaceHit == INFINITE_PLANE_BACK_FACE)
    {
        *SurfaceNormal = VectorNegate(Plane->SurfaceNormal);
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
InfinitePlaneCheckBounds(
    _In_ PCINFINITE_PLANE Plane,
    _In_ PCMATRIX ModelToWorld,
    _In_ BOUNDING_BOX WorldAlignedBoundingBox,
    _Out_ PBOOL IsInsideBox
    )
{
    ASSERT(Plane != NULL);
    ASSERT(IsInsideBox != NULL);

    *IsInsideBox = TRUE;
    return ISTATUS_SUCCESS;
}

//
// Types
//

typedef struct _PHYSX_INFINITE_PLANE {
    PPBR_MATERIAL Materials[2];
    INFINITE_PLANE Data;
} PHYSX_INFINITE_PLANE, *PPHYSX_INFINITE_PLANE;

typedef CONST PHYSX_INFINITE_PLANE *PCPHYSX_INFINITE_PLANE;

typedef struct _PHYSX_LIGHT_INFINITE_PLANE {
    PPBR_MATERIAL Materials[2];
    PPBR_LIGHT Lights[2];
    INFINITE_PLANE Data;
} PHYSX_LIGHT_INFINITE_PLANE, *PPHYSX_LIGHT_INFINITE_PLANE;

typedef CONST PHYSX_LIGHT_INFINITE_PLANE *PCPHYSX_LIGHT_INFINITE_PLANE;

//
// Static Functions
//

_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS 
PhysxInfinitePlaneGetMaterial(
    _In_ PCVOID Context, 
    _In_ UINT32 FaceHit,
    _Out_opt_ PCPBR_MATERIAL *Material
    )
{
    PCPHYSX_INFINITE_PLANE InfinitePlane;

    ASSERT(Context != NULL);
    ASSERT(Material != NULL);

    InfinitePlane = (PCPHYSX_INFINITE_PLANE) Context;

    if (FaceHit > INFINITE_PLANE_BACK_FACE)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }
    
    *Material = InfinitePlane->Materials[FaceHit];

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS
PhysxInfinitePlaneComputeNormal(
    _In_ PCVOID Context, 
    _In_ POINT3 ModelHitPoint,
    _In_ UINT32 FaceHit,
    _Out_ PVECTOR3 SurfaceNormal
    )
{
    PCPHYSX_INFINITE_PLANE InfinitePlane;
    ISTATUS Status;

    ASSERT(Context != NULL);
    ASSERT(SurfaceNormal != NULL);

    InfinitePlane = (PCPHYSX_INFINITE_PLANE) Context;
    
    Status = InfinitePlaneComputeNormal(&InfinitePlane->Data,
                                        ModelHitPoint,
                                        FaceHit,
                                        SurfaceNormal);
           
    return Status;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS 
PhysxInfinitePlaneTestBounds(
    _In_ PCVOID Context,
    _In_ PCMATRIX ModelToWorld,
    _In_ BOUNDING_BOX WorldAlignedBoundingBox,
    _Out_ PBOOL IsInsideBox
    )
{
    PCPHYSX_INFINITE_PLANE InfinitePlane;
    ISTATUS Status;

    ASSERT(Context != NULL);
    ASSERT(IsInsideBox != NULL);

    InfinitePlane = (PCPHYSX_INFINITE_PLANE) Context;
    
    Status = InfinitePlaneCheckBounds(&InfinitePlane->Data,
                                      ModelToWorld,
                                      WorldAlignedBoundingBox,
                                      IsInsideBox);
                                 
    return Status;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS 
PhysxInfinitePlaneTestRay(
    _In_opt_ PCVOID Context, 
    _In_ RAY Ray,
    _Inout_ PPBR_HIT_ALLOCATOR HitAllocator,
    _Outptr_result_maybenull_ PHIT_LIST *HitList
    )
{
    PCPHYSX_INFINITE_PLANE InfinitePlane;
    ISTATUS Status;

    ASSERT(Context != NULL);
    ASSERT(RayValidate(Ray) != FALSE);
    ASSERT(HitAllocator != NULL);
    ASSERT(HitList != NULL);

    InfinitePlane = (PCPHYSX_INFINITE_PLANE) Context;

    Status = InfinitePlaneTestRay(&InfinitePlane->Data,
                                  Ray,
                                  HitAllocator,
                                  HitList);

    return Status;
}

STATIC
VOID
PhysxInfinitePlaneFree(
    _In_ _Post_invalid_ PVOID Context
    )
{
    PCPHYSX_INFINITE_PLANE InfinitePlane;

    ASSERT(Context != NULL);

    InfinitePlane = (PCPHYSX_INFINITE_PLANE) Context;

    PbrMaterialRelease(InfinitePlane->Materials[INFINITE_PLANE_FRONT_FACE]);
    PbrMaterialRelease(InfinitePlane->Materials[INFINITE_PLANE_BACK_FACE]);
}

_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS 
PhysxLightInfinitePlaneGetMaterial(
    _In_ PCVOID Context, 
    _In_ UINT32 FaceHit,
    _Out_opt_ PCPBR_MATERIAL *Material
    )
{
    PCPHYSX_LIGHT_INFINITE_PLANE InfinitePlane;

    ASSERT(Context != NULL);

    InfinitePlane = (PCPHYSX_LIGHT_INFINITE_PLANE) Context;
    
    if (FaceHit > INFINITE_PLANE_BACK_FACE)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }
    
    *Material = InfinitePlane->Materials[FaceHit];

    return ISTATUS_SUCCESS;
}

_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS
PhysxLightInfinitePlaneGetLight(
    _In_opt_ PCVOID Context, 
    _In_ UINT32 FaceHit,
    _Out_ PCPBR_LIGHT *Light
    )
{
    PCPHYSX_LIGHT_INFINITE_PLANE InfinitePlane;

    ASSERT(Context != NULL);
    ASSERT(Light != NULL);

    InfinitePlane = (PCPHYSX_LIGHT_INFINITE_PLANE) Context;
    
    if (FaceHit > INFINITE_PLANE_BACK_FACE)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }
    
    *Light = InfinitePlane->Lights[FaceHit];

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS
PhysxLightInfinitePlaneComputeNormal(
    _In_ PCVOID Context, 
    _In_ POINT3 ModelHitPoint,
    _In_ UINT32 FaceHit,
    _Out_ PVECTOR3 SurfaceNormal
    )
{
    PCPHYSX_LIGHT_INFINITE_PLANE InfinitePlane;
    ISTATUS Status;

    ASSERT(Context != NULL);
    ASSERT(SurfaceNormal != NULL);

    InfinitePlane = (PCPHYSX_LIGHT_INFINITE_PLANE) Context;
    
    Status = InfinitePlaneComputeNormal(&InfinitePlane->Data,
                                        ModelHitPoint,
                                        FaceHit,
                                        SurfaceNormal);
                                   
    return Status;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS 
PhysxLightInfinitePlaneTestBounds(
    _In_ PCVOID Context,
    _In_ PCMATRIX ModelToWorld,
    _In_ BOUNDING_BOX WorldAlignedBoundingBox,
    _Out_ PBOOL IsInsideBox
    )
{
    PCPHYSX_LIGHT_INFINITE_PLANE InfinitePlane;
    ISTATUS Status;

    ASSERT(Context != NULL);
    ASSERT(IsInsideBox != NULL);

    InfinitePlane = (PCPHYSX_LIGHT_INFINITE_PLANE) Context;
    
    Status = InfinitePlaneCheckBounds(&InfinitePlane->Data,
                                      ModelToWorld,
                                      WorldAlignedBoundingBox,
                                      IsInsideBox);
                                 
    return Status;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS 
PhysxLightInfinitePlaneTestRay(
    _In_opt_ PCVOID Context, 
    _In_ RAY Ray,
    _Inout_ PPBR_HIT_ALLOCATOR HitAllocator,
    _Outptr_result_maybenull_ PHIT_LIST *HitList
    )
{
    PCPHYSX_LIGHT_INFINITE_PLANE InfinitePlane;
    ISTATUS Status;

    ASSERT(Context != NULL);
    ASSERT(RayValidate(Ray) != FALSE);
    ASSERT(HitAllocator != NULL);
    ASSERT(HitList != NULL);

    InfinitePlane = (PCPHYSX_LIGHT_INFINITE_PLANE) Context;

    Status = InfinitePlaneTestRay(&InfinitePlane->Data,
                                  Ray,
                                  HitAllocator,
                                  HitList);

    return Status;
}

STATIC
VOID
PhysxLightInfinitePlaneFree(
    _In_ _Post_invalid_ PVOID Context
    )
{
    PCPHYSX_LIGHT_INFINITE_PLANE InfinitePlane;

    ASSERT(Context != NULL);

    InfinitePlane = (PCPHYSX_LIGHT_INFINITE_PLANE) Context;

    PbrMaterialRelease(InfinitePlane->Materials[INFINITE_PLANE_FRONT_FACE]);
    PbrMaterialRelease(InfinitePlane->Materials[INFINITE_PLANE_BACK_FACE]);
    PbrLightRelease(InfinitePlane->Lights[INFINITE_PLANE_FRONT_FACE]);
    PbrLightRelease(InfinitePlane->Lights[INFINITE_PLANE_BACK_FACE]);
}

//
// Static Variables
//

CONST STATIC PBR_GEOMETRY_VTABLE InfinitePlaneHeader = {
    PhysxInfinitePlaneTestRay,
    PhysxInfinitePlaneComputeNormal,
    PhysxInfinitePlaneTestBounds,
    PhysxInfinitePlaneGetMaterial,
    NULL,
    PhysxInfinitePlaneFree
};

CONST STATIC PBR_GEOMETRY_VTABLE LightInfinitePlaneHeader = {
    PhysxLightInfinitePlaneTestRay,
    PhysxLightInfinitePlaneComputeNormal,
    PhysxLightInfinitePlaneTestBounds,
    PhysxLightInfinitePlaneGetMaterial,
    PhysxLightInfinitePlaneGetLight,
    PhysxLightInfinitePlaneFree
};

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PhysxInfinitePlaneAllocate(
    _In_ POINT3 Point,
    _In_ VECTOR3 SurfaceNormal,
    _In_opt_ PPBR_MATERIAL FrontMaterial,
    _In_opt_ PPBR_MATERIAL BackMaterial,
    _In_opt_ PPBR_LIGHT FrontLight,
    _In_opt_ PPBR_LIGHT BackLight,
    _Out_ PPBR_GEOMETRY *Geometry
    )
{
    PCPBR_GEOMETRY_VTABLE GeometryVTable;
    PHYSX_LIGHT_INFINITE_PLANE LightInfinitePlane;
    PHYSX_INFINITE_PLANE InfinitePlane;
    SIZE_T DataAlignment;
    SIZE_T DataSize;
    ISTATUS Status;
    PCVOID Data;
    
    if (Geometry == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_07;
    }

    if (FrontLight != NULL || 
        BackLight != NULL)
    {
        Status = InfinitePlaneInitialize(Point,
                                         SurfaceNormal,
                                         &LightInfinitePlane.Data);

        if (Status != ISTATUS_SUCCESS)
        {
            return Status;
        }
        
        LightInfinitePlane.Materials[INFINITE_PLANE_FRONT_FACE] = FrontMaterial;
        LightInfinitePlane.Materials[INFINITE_PLANE_BACK_FACE] = BackMaterial;
        LightInfinitePlane.Lights[INFINITE_PLANE_FRONT_FACE] = FrontLight;
        LightInfinitePlane.Lights[INFINITE_PLANE_BACK_FACE] = BackLight;
        
        GeometryVTable = &LightInfinitePlaneHeader;
        Data = &LightInfinitePlane;
        DataSize = sizeof(PHYSX_LIGHT_INFINITE_PLANE);
        DataAlignment = _Alignof(PHYSX_LIGHT_INFINITE_PLANE);
    }
    else
    {
        Status = InfinitePlaneInitialize(Point,
                                         SurfaceNormal,
                                         &InfinitePlane.Data);

        if (Status != ISTATUS_SUCCESS)
        {
            return Status;
        }
        
        InfinitePlane.Materials[INFINITE_PLANE_FRONT_FACE] = FrontMaterial;
        InfinitePlane.Materials[INFINITE_PLANE_BACK_FACE] = BackMaterial;

        GeometryVTable = &InfinitePlaneHeader;
        Data = &InfinitePlane;
        DataSize = sizeof(PHYSX_INFINITE_PLANE);
        DataAlignment = _Alignof(PHYSX_INFINITE_PLANE);
    }
    
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
    PbrMaterialRetain(BackMaterial);
    PbrLightRetain(FrontLight);
    PbrLightRetain(BackLight);

    return ISTATUS_SUCCESS;
}