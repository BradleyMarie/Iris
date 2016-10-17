/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    triangle.c

Abstract:

    This file contains the definitions for triangle types.

--*/

#include <irisphysxtoolkitp.h>

//
// Constants
//

#define TRIANGLE_DEGENERATE_THRESHOLD (FLOAT) 0.001
#define TRIANGLE_FRONT_FACE 0
#define TRIANGLE_BACK_FACE  1

//
// Types
//

typedef struct _TRIANGLE {
    POINT3 Vertex0;
    VECTOR3 B;
    VECTOR3 C;
    VECTOR3 SurfaceNormal;
} TRIANGLE, *PTRIANGLE;

typedef CONST TRIANGLE *PCTRIANGLE;

//
// Triangle Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
TriangleXDominantTestRay(
    _In_ PCTRIANGLE Triangle,
    _In_ RAY Ray,
    _Inout_ PPBR_HIT_ALLOCATOR HitAllocator,
    _Outptr_result_maybenull_ PHIT_LIST *HitList
    )
{
    UINT32 BackFace;
    FLOAT BarycentricCoordinates[3];
    FLOAT Distance;
    FLOAT DotProduct;
    UINT32 FrontFace;
    POINT3 Hit;
    VECTOR3 P;
    ISTATUS Status;
    VECTOR3 Temp;

    ASSERT(Triangle != NULL);
    ASSERT(RayValidate(Ray) != FALSE);
    ASSERT(HitAllocator != NULL);
    ASSERT(HitList != NULL);

    Temp = PointSubtract(Ray.Origin, Triangle->Vertex0);

    DotProduct = VectorDotProduct(Ray.Direction, Triangle->SurfaceNormal);
    Distance = VectorDotProduct(Temp, Triangle->SurfaceNormal) / -DotProduct;

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

    Hit = RayEndpoint(Ray, Distance);
    P = PointSubtract(Hit, Triangle->Vertex0);

    BarycentricCoordinates[1] = (P.Z * Triangle->C.Y -
                                 P.Y * Triangle->C.Z) /
        (Triangle->B.Z * Triangle->C.Y -
         Triangle->B.Y * Triangle->C.Z);

    if (IsGreaterThanOrEqualToZeroFloat(BarycentricCoordinates[1]) == FALSE)
    {
        *HitList = NULL;
        return ISTATUS_SUCCESS;
    }

    BarycentricCoordinates[2] = (P.Z * Triangle->B.Y -
                                 P.Y * Triangle->B.Z) /
        (Triangle->C.Z * Triangle->B.Y -
         Triangle->C.Y * Triangle->B.Z);

    if (IsGreaterThanOrEqualToZeroFloat(BarycentricCoordinates[2]) == FALSE)
    {
        *HitList = NULL;
        return ISTATUS_SUCCESS;
    }

    BarycentricCoordinates[0] = (FLOAT) 1.0 -
        BarycentricCoordinates[1] -
        BarycentricCoordinates[2];

    if (IsGreaterThanOrEqualToZeroFloat(BarycentricCoordinates[0]) == FALSE)
    {
        *HitList = NULL;
        return ISTATUS_SUCCESS;
    }

    if ((FLOAT) 0.0 > DotProduct)
    {
        FrontFace = TRIANGLE_FRONT_FACE;
        BackFace = TRIANGLE_BACK_FACE;
    }
    else
    {
        FrontFace = TRIANGLE_BACK_FACE;
        BackFace = TRIANGLE_FRONT_FACE;
    }

    Status = PBRHitAllocatorAllocateWithHitPoint(HitAllocator,
                                                 NULL,
                                                 Distance,
                                                 FrontFace,
                                                 BackFace,
                                                 &BarycentricCoordinates,
                                                 sizeof(BarycentricCoordinates),
                                                 _Alignof(FLOAT),
                                                 Hit,
                                                 HitList);

    return Status;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
TriangleYDominantTestRay(
    _In_ PCTRIANGLE Triangle,
    _In_ RAY Ray,
    _Inout_ PPBR_HIT_ALLOCATOR HitAllocator,
    _Outptr_result_maybenull_ PHIT_LIST *HitList
    )
{
    UINT32 BackFace;
    FLOAT BarycentricCoordinates[3];
    FLOAT Distance;
    FLOAT DotProduct;
    UINT32 FrontFace;
    POINT3 Hit;
    VECTOR3 P;
    ISTATUS Status;
    VECTOR3 Temp;

    ASSERT(Triangle != NULL);
    ASSERT(RayValidate(Ray) != FALSE);
    ASSERT(HitAllocator != NULL);
    ASSERT(HitList != NULL);

    Temp = PointSubtract(Ray.Origin, Triangle->Vertex0);

    DotProduct = VectorDotProduct(Ray.Direction, Triangle->SurfaceNormal);
    Distance = VectorDotProduct(Temp, Triangle->SurfaceNormal) / -DotProduct;

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

    Hit = RayEndpoint(Ray, Distance);
    P = PointSubtract(Hit, Triangle->Vertex0);

    BarycentricCoordinates[1] = (P.X * Triangle->C.Z -
                                 P.Z * Triangle->C.X) /
        (Triangle->B.X * Triangle->C.Z -
         Triangle->B.Z * Triangle->C.X);

    if (IsGreaterThanOrEqualToZeroFloat(BarycentricCoordinates[1]) == FALSE)
    {
        *HitList = NULL;
        return ISTATUS_SUCCESS;
    }

    BarycentricCoordinates[2] = (P.X * Triangle->B.Z -
                                 P.Z * Triangle->B.X) /
        (Triangle->C.X * Triangle->B.Z -
         Triangle->C.Z * Triangle->B.X);

    if (IsGreaterThanOrEqualToZeroFloat(BarycentricCoordinates[2]) == FALSE)
    {
        *HitList = NULL;
        return ISTATUS_SUCCESS;
    }

    BarycentricCoordinates[0] = (FLOAT) 1.0 -
        BarycentricCoordinates[1] -
        BarycentricCoordinates[2];

    if (IsGreaterThanOrEqualToZeroFloat(BarycentricCoordinates[0]) == FALSE)
    {
        *HitList = NULL;
        return ISTATUS_SUCCESS;
    }
    
    if ((FLOAT) 0.0 > DotProduct)
    {
        FrontFace = TRIANGLE_FRONT_FACE;
        BackFace = TRIANGLE_BACK_FACE;
    }
    else
    {
        FrontFace = TRIANGLE_BACK_FACE;
        BackFace = TRIANGLE_FRONT_FACE;
    }
    
    Status = PBRHitAllocatorAllocateWithHitPoint(HitAllocator,
                                                 NULL,
                                                 Distance,
                                                 FrontFace,
                                                 BackFace,
                                                 &BarycentricCoordinates,
                                                 sizeof(BarycentricCoordinates),
                                                 _Alignof(FLOAT),
                                                 Hit,
                                                 HitList);

    return Status;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
TriangleZDominantTestRay(
    _In_ PCTRIANGLE Triangle,
    _In_ RAY Ray,
    _Inout_ PPBR_HIT_ALLOCATOR HitAllocator,
    _Outptr_result_maybenull_ PHIT_LIST *HitList
    )
{
    UINT32 BackFace;
    FLOAT BarycentricCoordinates[3];
    FLOAT Distance;
    FLOAT DotProduct;
    UINT32 FrontFace;
    POINT3 Hit;
    VECTOR3 P;
    ISTATUS Status;
    VECTOR3 Temp;

    ASSERT(Triangle != NULL);
    ASSERT(RayValidate(Ray) != FALSE);
    ASSERT(HitAllocator != NULL);
    ASSERT(HitList != NULL);

    Temp = PointSubtract(Ray.Origin, Triangle->Vertex0);

    DotProduct = VectorDotProduct(Ray.Direction, Triangle->SurfaceNormal);
    Distance = VectorDotProduct(Temp, Triangle->SurfaceNormal) / -DotProduct;

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

    Hit = RayEndpoint(Ray, Distance);
    P = PointSubtract(Hit, Triangle->Vertex0);

    BarycentricCoordinates[1] = (P.Y * Triangle->C.X -
                                 P.X * Triangle->C.Y) /
        (Triangle->B.Y * Triangle->C.X -
         Triangle->B.X * Triangle->C.Y);

    if (IsGreaterThanOrEqualToZeroFloat(BarycentricCoordinates[1]) == FALSE)
    {
        *HitList = NULL;
        return ISTATUS_SUCCESS;
    }

    BarycentricCoordinates[2] = (P.Y * Triangle->B.X -
                                 P.X * Triangle->B.Y) /
        (Triangle->C.Y * Triangle->B.X -
         Triangle->C.X * Triangle->B.Y);

    if (IsGreaterThanOrEqualToZeroFloat(BarycentricCoordinates[2]) == FALSE)
    {
        *HitList = NULL;
        return ISTATUS_SUCCESS;
    }

    BarycentricCoordinates[0] = (FLOAT) 1.0 -
        BarycentricCoordinates[1] -
        BarycentricCoordinates[2];

    if (IsGreaterThanOrEqualToZeroFloat(BarycentricCoordinates[0]) == FALSE)
    {
        *HitList = NULL;
        return ISTATUS_SUCCESS;
    }
    
    if ((FLOAT) 0.0 > DotProduct)
    {
        FrontFace = TRIANGLE_FRONT_FACE;
        BackFace = TRIANGLE_BACK_FACE;
    }
    else
    {
        FrontFace = TRIANGLE_BACK_FACE;
        BackFace = TRIANGLE_FRONT_FACE;
    }
    
    Status = PBRHitAllocatorAllocateWithHitPoint(HitAllocator,
                                                 NULL,
                                                 Distance,
                                                 FrontFace,
                                                 BackFace,
                                                 &BarycentricCoordinates,
                                                 sizeof(BarycentricCoordinates),
                                                 _Alignof(FLOAT),
                                                 Hit,
                                                 HitList);

    return Status;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
TriangleInitialize(
    _Out_ PTRIANGLE Triangle,
    _In_ POINT3 Vertex0,
    _In_ POINT3 Vertex1,
    _In_ POINT3 Vertex2,
    _Out_ PVECTOR_AXIS DominantAxis
    )
{
    VECTOR3 B;
    VECTOR3 C;
    VECTOR3 CrossProduct;
    FLOAT CrossProductLength;

    ASSERT(Triangle != NULL);
    ASSERT(PointValidate(Vertex0) != FALSE);
    ASSERT(PointValidate(Vertex1) != FALSE);
    ASSERT(PointValidate(Vertex2) != FALSE);
    ASSERT(DominantAxis != NULL);

    B = PointSubtract(Vertex1, Vertex0);
    C = PointSubtract(Vertex2, Vertex0);

    CrossProduct = VectorCrossProduct(C, B);
    CrossProductLength = VectorLength(CrossProduct);

    if (CrossProductLength <= TRIANGLE_DEGENERATE_THRESHOLD)
    {
        return ISTATUS_INVALID_ARGUMENT_COMBINATION_00;
    }

    Triangle->Vertex0 = Vertex0;
    Triangle->B = B;
    Triangle->C = C;

    Triangle->SurfaceNormal = VectorNormalize(CrossProduct, NULL, NULL);
    *DominantAxis = VectorDominantAxis(Triangle->SurfaceNormal);

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
TriangleComputeNormal(
    _In_ PCTRIANGLE Triangle,
    _In_ POINT3 HitPoint,
    _In_ UINT32 FaceHit,
    _Out_ PVECTOR3 SurfaceNormal
    )
{
    ASSERT(Triangle != NULL);
    ASSERT(PointValidate(HitPoint) != FALSE);
    ASSERT(SurfaceNormal != NULL);

    if (FaceHit == TRIANGLE_FRONT_FACE)
    {
        *SurfaceNormal = Triangle->SurfaceNormal;
    }
    else if (FaceHit == TRIANGLE_BACK_FACE)
    {
        *SurfaceNormal = VectorNegate(Triangle->SurfaceNormal);
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

//
// Types
//

typedef struct _PHYSX_TRIANGLE {
    PPBR_MATERIAL Materials[2];
    TRIANGLE Data;
} PHYSX_TRIANGLE, *PPHYSX_TRIANGLE;

typedef CONST PHYSX_TRIANGLE *PCPHYSX_TRIANGLE;

typedef struct _PHYSX_LIGHT_TRIANGLE {
    PPBR_MATERIAL Materials[2];
    PPBR_LIGHT Lights[2];
    TRIANGLE Data;
} PHYSX_LIGHT_TRIANGLE, *PPHYSX_LIGHT_TRIANGLE;

typedef CONST PHYSX_LIGHT_TRIANGLE *PCPHYSX_LIGHT_TRIANGLE;

//
// Static Functions
//

_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS 
PhysxTriangleGetMaterial(
    _In_ PCVOID Context, 
    _In_ UINT32 FaceHit,
    _Out_opt_ PCPBR_MATERIAL *Material
    )
{
    PCPHYSX_TRIANGLE Triangle;

    ASSERT(Context != NULL);
    ASSERT(Material != NULL);

    Triangle = (PCPHYSX_TRIANGLE) Context;

    if (FaceHit > TRIANGLE_BACK_FACE)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }
    
    *Material = Triangle->Materials[FaceHit];

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS
PhysxTriangleComputeNormal(
    _In_ PCVOID Context, 
    _In_ POINT3 ModelHitPoint,
    _In_ UINT32 FaceHit,
    _Out_ PVECTOR3 SurfaceNormal
    )
{
    PCPHYSX_TRIANGLE Triangle;
    ISTATUS Status;

    ASSERT(Context != NULL);
    ASSERT(SurfaceNormal != NULL);

    Triangle = (PCPHYSX_TRIANGLE) Context;
    
    Status = TriangleComputeNormal(&Triangle->Data,
                                   ModelHitPoint,
                                   FaceHit,
                                   SurfaceNormal);
                                   
    return Status;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS 
PhysxTriangleTestBounds(
    _In_ PCVOID Context,
    _In_ PCMATRIX ModelToWorld,
    _In_ BOUNDING_BOX WorldAlignedBoundingBox,
    _Out_ PBOOL IsInsideBox
    )
{
    PCPHYSX_TRIANGLE Triangle;
    ISTATUS Status;

    ASSERT(Context != NULL);
    ASSERT(IsInsideBox != NULL);

    Triangle = (PCPHYSX_TRIANGLE) Context;
    
    Status = TriangleCheckBounds(&Triangle->Data,
                                 ModelToWorld,
                                 WorldAlignedBoundingBox,
                                 IsInsideBox);
                                 
    return Status;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS 
PhysxTriangleXDominantTestRay(
    _In_opt_ PCVOID Context, 
    _In_ RAY Ray,
    _Inout_ PPBR_HIT_ALLOCATOR HitAllocator,
    _Outptr_result_maybenull_ PHIT_LIST *HitList
    )
{
    PCPHYSX_TRIANGLE Triangle;
    ISTATUS Status;

    ASSERT(Context != NULL);
    ASSERT(RayValidate(Ray) != FALSE);
    ASSERT(HitAllocator != NULL);
    ASSERT(HitList != NULL);

    Triangle = (PCPHYSX_TRIANGLE) Context;

    Status = TriangleXDominantTestRay(&Triangle->Data,
                                      Ray,
                                      HitAllocator,
                                      HitList);

    return Status;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS 
PhysxTriangleYDominantTestRay(
    _In_opt_ PCVOID Context, 
    _In_ RAY Ray,
    _Inout_ PPBR_HIT_ALLOCATOR HitAllocator,
    _Outptr_result_maybenull_ PHIT_LIST *HitList
    )
{
    PCPHYSX_TRIANGLE Triangle;
    ISTATUS Status;

    ASSERT(Context != NULL);
    ASSERT(RayValidate(Ray) != FALSE);
    ASSERT(HitAllocator != NULL);
    ASSERT(HitList != NULL);

    Triangle = (PCPHYSX_TRIANGLE) Context;

    Status = TriangleYDominantTestRay(&Triangle->Data,
                                      Ray,
                                      HitAllocator,
                                      HitList);

    return Status;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS 
PhysxTriangleZDominantTestRay(
    _In_opt_ PCVOID Context, 
    _In_ RAY Ray,
    _Inout_ PPBR_HIT_ALLOCATOR HitAllocator,
    _Outptr_result_maybenull_ PHIT_LIST *HitList
    )
{
    PCPHYSX_TRIANGLE Triangle;
    ISTATUS Status;

    ASSERT(Context != NULL);
    ASSERT(RayValidate(Ray) != FALSE);
    ASSERT(HitAllocator != NULL);
    ASSERT(HitList != NULL);

    Triangle = (PCPHYSX_TRIANGLE) Context;

    Status = TriangleZDominantTestRay(&Triangle->Data,
                                      Ray,
                                      HitAllocator,
                                      HitList);

    return Status;
}

STATIC
VOID
PhysxTriangleFree(
    _In_ _Post_invalid_ PVOID Context
    )
{
    PCPHYSX_TRIANGLE Triangle;

    ASSERT(Context != NULL);

    Triangle = (PCPHYSX_TRIANGLE) Context;

    PbrMaterialRelease(Triangle->Materials[TRIANGLE_FRONT_FACE]);
    PbrMaterialRelease(Triangle->Materials[TRIANGLE_BACK_FACE]);
}

//
// Static Variables
//

CONST STATIC PBR_GEOMETRY_VTABLE XTriangleHeader = {
    PhysxTriangleXDominantTestRay,
    PhysxTriangleComputeNormal,
    PhysxTriangleTestBounds,
    PhysxTriangleGetMaterial,
    PhysxTriangleFree
};

CONST STATIC PBR_GEOMETRY_VTABLE YTriangleHeader = {
    PhysxTriangleYDominantTestRay,
    PhysxTriangleComputeNormal,
    PhysxTriangleTestBounds,
    PhysxTriangleGetMaterial,
	PhysxTriangleFree
};

CONST STATIC PBR_GEOMETRY_VTABLE ZTriangleHeader = {
    PhysxTriangleZDominantTestRay,
    PhysxTriangleComputeNormal,
    PhysxTriangleTestBounds,
    PhysxTriangleGetMaterial,
	PhysxTriangleFree
};

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PhysxTriangleAllocate(
    _In_ POINT3 Vertex0,
    _In_ POINT3 Vertex1,
    _In_ POINT3 Vertex2,
    _In_opt_ PPBR_MATERIAL FrontMaterial,
    _In_opt_ PPBR_MATERIAL BackMaterial,
    _Out_ PPBR_GEOMETRY *Geometry
    )
{
    PCPBR_GEOMETRY_VTABLE GeometryVTable;
    VECTOR_AXIS DominantAxis;
    PHYSX_TRIANGLE Triangle;
    SIZE_T DataAlignment;
    SIZE_T DataSize;
    ISTATUS Status;
    PCVOID Data;

    if (PointValidate(Vertex0) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }
    
    if (PointValidate(Vertex1) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }
    
    if (PointValidate(Vertex2) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }
    
    if (Geometry == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_07;
    }


	Status = TriangleInitialize(&Triangle.Data,
								Vertex0,
								Vertex1,
								Vertex2,
								&DominantAxis);

	if (Status != ISTATUS_SUCCESS)
	{
		return Status;
	}

	Triangle.Materials[TRIANGLE_FRONT_FACE] = FrontMaterial;
	Triangle.Materials[TRIANGLE_BACK_FACE] = BackMaterial;

	Data = &Triangle;
	DataSize = sizeof(PHYSX_TRIANGLE);
	DataAlignment = _Alignof(PHYSX_TRIANGLE);

	switch (DominantAxis)
	{
		case VECTOR_X_AXIS:
			GeometryVTable = &XTriangleHeader;
			break;
		case VECTOR_Y_AXIS:
			GeometryVTable = &YTriangleHeader;
			break;
		default:
			GeometryVTable = &ZTriangleHeader;
			break;
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

    return ISTATUS_SUCCESS;
}
