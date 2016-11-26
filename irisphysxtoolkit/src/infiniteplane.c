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
    _Inout_ PPHYSX_HIT_ALLOCATOR HitAllocator,
    _Outptr_result_maybenull_ PHIT_LIST *HitList
    )
{
    UINT32 BackFace;
    FLOAT Distance;
    FLOAT DotProduct;
    UINT32 FrontFace;
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

    if ((FLOAT) 0.0 > DotProduct)
    {
        FrontFace = INFINITE_PLANE_FRONT_FACE;
        BackFace = INFINITE_PLANE_BACK_FACE;
    }
    else
    {
        FrontFace = INFINITE_PLANE_BACK_FACE;
        BackFace = INFINITE_PLANE_FRONT_FACE;
    }

    Status = PhysxHitAllocatorAllocate(HitAllocator,
                                       NULL,
                                       Distance,
                                       FrontFace,
                                       BackFace,
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
    PPHYSX_MATERIAL Materials[2];
    INFINITE_PLANE Data;
} PHYSX_INFINITE_PLANE, *PPHYSX_INFINITE_PLANE;

typedef CONST PHYSX_INFINITE_PLANE *PCPHYSX_INFINITE_PLANE;

typedef struct _PHYSX_LIGHT_INFINITE_PLANE {
    PPHYSX_MATERIAL Materials[2];
    PPHYSX_LIGHT Lights[2];
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
    _Out_opt_ PCPHYSX_MATERIAL *Material
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
    _In_ UINT32 FaceHit,
    _In_ POINT3 ModelHitPoint,
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
    _Inout_ PPHYSX_HIT_ALLOCATOR HitAllocator,
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

    PhysxMaterialRelease(InfinitePlane->Materials[INFINITE_PLANE_FRONT_FACE]);
    PhysxMaterialRelease(InfinitePlane->Materials[INFINITE_PLANE_BACK_FACE]);
}

//
// Static Variables
//

CONST STATIC PHYSX_GEOMETRY_VTABLE InfinitePlaneHeader = {
    PhysxInfinitePlaneTestRay,
    PhysxInfinitePlaneComputeNormal,
    PhysxInfinitePlaneTestBounds,
    PhysxInfinitePlaneGetMaterial,
    PhysxInfinitePlaneFree
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
    _In_opt_ PPHYSX_MATERIAL FrontMaterial,
    _In_opt_ PPHYSX_MATERIAL BackMaterial,
    _Out_ PPHYSX_GEOMETRY *Geometry
    )
{
    PCPHYSX_GEOMETRY_VTABLE GeometryVTable;
    PHYSX_INFINITE_PLANE InfinitePlane;
    SIZE_T DataAlignment;
    SIZE_T DataSize;
    ISTATUS Status;
    PCVOID Data;
    
    if (Geometry == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_07;
    }

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
    
    Status = PhysxGeometryAllocate(GeometryVTable,
                                   Data,
                                   DataSize,
                                   DataAlignment,
                                   Geometry);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    PhysxMaterialRetain(FrontMaterial);
    PhysxMaterialRetain(BackMaterial);

    return ISTATUS_SUCCESS;
}
