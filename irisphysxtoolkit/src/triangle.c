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
    _Inout_ PPHYSX_HIT_ALLOCATOR HitAllocator,
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

    Status = PhysxHitAllocatorAllocateWithHitPoint(HitAllocator,
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
    _Inout_ PPHYSX_HIT_ALLOCATOR HitAllocator,
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
    
    Status = PhysxHitAllocatorAllocateWithHitPoint(HitAllocator,
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
    _Inout_ PPHYSX_HIT_ALLOCATOR HitAllocator,
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
    
    Status = PhysxHitAllocatorAllocateWithHitPoint(HitAllocator,
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
    _In_ UINT32 FaceHit,
    _In_ POINT3 HitPoint,
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
    PPHYSX_MATERIAL Materials[2];
    TRIANGLE Data;
} PHYSX_TRIANGLE, *PPHYSX_TRIANGLE;

typedef CONST PHYSX_TRIANGLE *PCPHYSX_TRIANGLE;

typedef struct _PHYSX_LIGHTED_TRIANGLE {
    PPHYSX_MATERIAL Materials[2];
    TRIANGLE Data;
    FLOAT SurfaceArea;
} PHYSX_LIGHTED_TRIANGLE, *PPHYSX_LIGHTED_TRIANGLE;

typedef CONST PHYSX_LIGHTED_TRIANGLE *PCPHYSX_LIGHTED_TRIANGLE;

//
// Triangle Static Functions
//

_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS 
PhysxTriangleGetMaterial(
    _In_ PCVOID Context, 
    _In_ UINT32 FaceHit,
    _Out_opt_ PCPHYSX_MATERIAL *Material
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
    _In_ UINT32 FaceHit,
    _In_ POINT3 ModelHitPoint,
    _Out_ PVECTOR3 SurfaceNormal
    )
{
    PCPHYSX_TRIANGLE Triangle;
    ISTATUS Status;

    ASSERT(Context != NULL);
    ASSERT(SurfaceNormal != NULL);

    Triangle = (PCPHYSX_TRIANGLE) Context;
    
    Status = TriangleComputeNormal(&Triangle->Data,
                                   FaceHit,
                                   ModelHitPoint,
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
    _Inout_ PPHYSX_HIT_ALLOCATOR HitAllocator,
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
    _Inout_ PPHYSX_HIT_ALLOCATOR HitAllocator,
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
    _Inout_ PPHYSX_HIT_ALLOCATOR HitAllocator,
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

    PhysxMaterialRelease(Triangle->Materials[TRIANGLE_FRONT_FACE]);
    PhysxMaterialRelease(Triangle->Materials[TRIANGLE_BACK_FACE]);
}

//
// Lighted Triangle Static Functions
//

_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS 
PhysxLightedTriangleGetMaterial(
    _In_ PCVOID Context, 
    _In_ UINT32 FaceHit,
    _Out_opt_ PCPHYSX_MATERIAL *Material
    )
{
    PCPHYSX_LIGHTED_TRIANGLE Triangle;

    ASSERT(Context != NULL);
    ASSERT(Material != NULL);

    Triangle = (PCPHYSX_LIGHTED_TRIANGLE) Context;

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
PhysxLightedTriangleComputeNormal(
    _In_ PCVOID Context,
    _In_ UINT32 FaceHit,
    _In_ POINT3 ModelHitPoint,
    _Out_ PVECTOR3 SurfaceNormal
    )
{
    PCPHYSX_LIGHTED_TRIANGLE Triangle;
    ISTATUS Status;

    ASSERT(Context != NULL);
    ASSERT(SurfaceNormal != NULL);

    Triangle = (PCPHYSX_LIGHTED_TRIANGLE) Context;
    
    Status = TriangleComputeNormal(&Triangle->Data,
                                   FaceHit,
                                   ModelHitPoint,
                                   SurfaceNormal);
                                   
    return Status;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS 
PhysxLightedTriangleTestBounds(
    _In_ PCVOID Context,
    _In_ PCMATRIX ModelToWorld,
    _In_ BOUNDING_BOX WorldAlignedBoundingBox,
    _Out_ PBOOL IsInsideBox
    )
{
    PCPHYSX_LIGHTED_TRIANGLE Triangle;
    ISTATUS Status;

    ASSERT(Context != NULL);
    ASSERT(IsInsideBox != NULL);

    Triangle = (PCPHYSX_LIGHTED_TRIANGLE) Context;
    
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
PhysxLightedTriangleComputeSurfaceArea(
    _In_opt_ PCVOID Context, 
    _In_ UINT32 Face,
    _Out_ PFLOAT SurfaceArea
    )
{
    PCPHYSX_LIGHTED_TRIANGLE Triangle;

    ASSERT(Context != NULL);
    ASSERT(SurfaceArea != NULL);

    if (Face > TRIANGLE_BACK_FACE)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    Triangle = (PCPHYSX_LIGHTED_TRIANGLE) Context;
    
    *SurfaceArea = Triangle->SurfaceArea;
                                 
    return ISTATUS_SUCCESS;
}

_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS
PhysxLightedTriangleSampleSurface(
    _In_opt_ PCVOID Context, 
    _In_ UINT32 Face,
    _Inout_ PRANDOM Rng,
    _Out_ PPOINT3 Sample
    )
{
    PCPHYSX_LIGHTED_TRIANGLE Triangle;
    POINT3 Vertex0;
    POINT3 Vertex1;
    POINT3 Vertex2;
    FLOAT SqrtRandom0;
    FLOAT Random0;
    FLOAT Random1;
    FLOAT Scalar0;
    FLOAT Scalar1;
    FLOAT Scalar2;
    FLOAT X;
    FLOAT Y;
    FLOAT Z;

    ASSERT(Context != NULL);
    ASSERT(Rng != NULL);
    ASSERT(Sample != NULL);

    if (Face > TRIANGLE_BACK_FACE)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    Triangle = (PCPHYSX_LIGHTED_TRIANGLE) Context;

    RandomGenerateFloat(Rng, 
                        (FLOAT) 0.0f, 
                        (FLOAT) 1.0f,
                        &Random0);

    SqrtRandom0 = SqrtFloat(Random0);

    RandomGenerateFloat(Rng, 
                        (FLOAT) 0.0f, 
                        (FLOAT) 1.0f,
                        &Random1);

    Vertex0 = Triangle->Data.Vertex0;
    Vertex1 = PointVectorAdd(Vertex0, Triangle->Data.B);
    Vertex2 = PointVectorAdd(Vertex0, Triangle->Data.C);

    Scalar0 = (FLOAT) 1.0f - SqrtRandom0;
    Scalar1 = SqrtRandom0 * ((FLOAT) 1.0f - Random1);
    Scalar2 = SqrtRandom0 * Random1;

    X = Scalar0 * Vertex0.X +
        Scalar1 * Vertex1.X +
        Scalar2 * Vertex2.X;

    Y = Scalar0 * Vertex0.Y +
        Scalar1 * Vertex1.Y +
        Scalar2 * Vertex2.Y;

    Z = Scalar0 * Vertex0.Z +
        Scalar1 * Vertex1.Z +
        Scalar2 * Vertex2.Z;

    *Sample = PointCreate(X, Y, Z);

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS 
PhysxLightedTriangleXDominantTestRay(
    _In_opt_ PCVOID Context, 
    _In_ RAY Ray,
    _Inout_ PPHYSX_HIT_ALLOCATOR HitAllocator,
    _Outptr_result_maybenull_ PHIT_LIST *HitList
    )
{
    PCPHYSX_LIGHTED_TRIANGLE Triangle;
    ISTATUS Status;

    ASSERT(Context != NULL);
    ASSERT(RayValidate(Ray) != FALSE);
    ASSERT(HitAllocator != NULL);
    ASSERT(HitList != NULL);

    Triangle = (PCPHYSX_LIGHTED_TRIANGLE) Context;

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
PhysxLightedTriangleYDominantTestRay(
    _In_opt_ PCVOID Context, 
    _In_ RAY Ray,
    _Inout_ PPHYSX_HIT_ALLOCATOR HitAllocator,
    _Outptr_result_maybenull_ PHIT_LIST *HitList
    )
{
    PCPHYSX_LIGHTED_TRIANGLE Triangle;
    ISTATUS Status;

    ASSERT(Context != NULL);
    ASSERT(RayValidate(Ray) != FALSE);
    ASSERT(HitAllocator != NULL);
    ASSERT(HitList != NULL);

    Triangle = (PCPHYSX_LIGHTED_TRIANGLE) Context;

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
PhysxLightedTriangleZDominantTestRay(
    _In_opt_ PCVOID Context, 
    _In_ RAY Ray,
    _Inout_ PPHYSX_HIT_ALLOCATOR HitAllocator,
    _Outptr_result_maybenull_ PHIT_LIST *HitList
    )
{
    PCPHYSX_LIGHTED_TRIANGLE Triangle;
    ISTATUS Status;

    ASSERT(Context != NULL);
    ASSERT(RayValidate(Ray) != FALSE);
    ASSERT(HitAllocator != NULL);
    ASSERT(HitList != NULL);

    Triangle = (PCPHYSX_LIGHTED_TRIANGLE) Context;

    Status = TriangleZDominantTestRay(&Triangle->Data,
                                      Ray,
                                      HitAllocator,
                                      HitList);

    return Status;
}

STATIC
VOID
PhysxLightedTriangleFree(
    _In_ _Post_invalid_ PVOID Context
    )
{
    PCPHYSX_LIGHTED_TRIANGLE Triangle;

    ASSERT(Context != NULL);

    Triangle = (PCPHYSX_LIGHTED_TRIANGLE) Context;

    PhysxMaterialRelease(Triangle->Materials[TRIANGLE_FRONT_FACE]);
    PhysxMaterialRelease(Triangle->Materials[TRIANGLE_BACK_FACE]);
}

//
// Static Variables
//

CONST STATIC PHYSX_GEOMETRY_VTABLE XTriangleHeader = {
    PhysxTriangleXDominantTestRay,
    PhysxTriangleComputeNormal,
    PhysxTriangleTestBounds,
    PhysxTriangleGetMaterial,
    PhysxTriangleFree
};

CONST STATIC PHYSX_GEOMETRY_VTABLE YTriangleHeader = {
    PhysxTriangleYDominantTestRay,
    PhysxTriangleComputeNormal,
    PhysxTriangleTestBounds,
    PhysxTriangleGetMaterial,
	PhysxTriangleFree
};

CONST STATIC PHYSX_GEOMETRY_VTABLE ZTriangleHeader = {
    PhysxTriangleZDominantTestRay,
    PhysxTriangleComputeNormal,
    PhysxTriangleTestBounds,
    PhysxTriangleGetMaterial,
	PhysxTriangleFree
};

CONST STATIC PHYSX_LIGHTED_GEOMETRY_VTABLE XLightedTriangleHeader = {
    { PhysxLightedTriangleXDominantTestRay,
      PhysxLightedTriangleComputeNormal,
      PhysxLightedTriangleTestBounds,
      PhysxLightedTriangleGetMaterial,
      PhysxLightedTriangleFree },
    PhysxLightedTriangleComputeSurfaceArea,
    PhysxLightedTriangleSampleSurface
};

CONST STATIC PHYSX_LIGHTED_GEOMETRY_VTABLE YLightedTriangleHeader = {
    { PhysxLightedTriangleYDominantTestRay,
      PhysxLightedTriangleComputeNormal,
      PhysxLightedTriangleTestBounds,
      PhysxLightedTriangleGetMaterial,
      PhysxLightedTriangleFree },
    PhysxLightedTriangleComputeSurfaceArea,
    PhysxLightedTriangleSampleSurface
};

CONST STATIC PHYSX_LIGHTED_GEOMETRY_VTABLE ZLightedTriangleHeader = {
    { PhysxLightedTriangleZDominantTestRay,
      PhysxLightedTriangleComputeNormal,
      PhysxLightedTriangleTestBounds,
      PhysxLightedTriangleGetMaterial,
      PhysxLightedTriangleFree },
    PhysxLightedTriangleComputeSurfaceArea,
    PhysxLightedTriangleSampleSurface
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
    _In_opt_ PPHYSX_MATERIAL FrontMaterial,
    _In_opt_ PPHYSX_MATERIAL BackMaterial,
    _Out_ PPHYSX_GEOMETRY *Geometry
    )
{
    PCPHYSX_GEOMETRY_VTABLE GeometryVTable;
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
        return ISTATUS_INVALID_ARGUMENT_05;
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

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PhysxLightedTriangleAllocate(
    _In_ PPHYSX_AREA_LIGHT_BUILDER Builder,
    _In_ POINT3 Vertex0,
    _In_ POINT3 Vertex1,
    _In_ POINT3 Vertex2,
    _In_opt_ PPHYSX_MATERIAL FrontMaterial,
    _In_opt_ PPHYSX_MATERIAL BackMaterial,
    _Out_ PSIZE_T AreaGeometryIndex
    )
{
    PCPHYSX_LIGHTED_GEOMETRY_VTABLE LightedGeometryVTable;
    VECTOR_AXIS DominantAxis;
    PHYSX_LIGHTED_TRIANGLE Triangle;
    FLOAT OneHalfPerimeter;
    SIZE_T DataAlignment;
    FLOAT SideALength;
    FLOAT SideBLength;
    FLOAT SideCLength;
    SIZE_T DataSize;
    ISTATUS Status;
    VECTOR3 SideA;
    PCVOID Data;

    if (Builder == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (PointValidate(Vertex0) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }
    
    if (PointValidate(Vertex1) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }
    
    if (PointValidate(Vertex2) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }
    
    if (AreaGeometryIndex == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_06;
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
	DataSize = sizeof(PHYSX_LIGHTED_TRIANGLE);
	DataAlignment = _Alignof(PHYSX_LIGHTED_TRIANGLE);

	switch (DominantAxis)
	{
		case VECTOR_X_AXIS:
			LightedGeometryVTable = &XLightedTriangleHeader;
			break;
		case VECTOR_Y_AXIS:
			LightedGeometryVTable = &YLightedTriangleHeader;
			break;
		default:
			LightedGeometryVTable = &ZLightedTriangleHeader;
			break;
	}

    SideA = PointSubtract(Vertex1, Vertex2);

    SideALength = VectorLength(SideA);
    SideBLength = VectorLength(Triangle.Data.B);
    SideCLength = VectorLength(Triangle.Data.C);
    
    OneHalfPerimeter = (FLOAT) 0.5f * (SideALength + SideBLength + SideCLength);

    Triangle.SurfaceArea = SqrtFloat(OneHalfPerimeter *
                                     OneHalfPerimeter * (OneHalfPerimeter - SideALength) *
                                     OneHalfPerimeter * (OneHalfPerimeter - SideBLength) *
                                     OneHalfPerimeter * (OneHalfPerimeter - SideBLength));

    Status = PhysxAreaLightBuilderAddGeometry(Builder,
                                              LightedGeometryVTable,
                                              Data,
                                              DataSize,
                                              DataAlignment,
                                              AreaGeometryIndex);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    PhysxMaterialRetain(FrontMaterial);
    PhysxMaterialRetain(BackMaterial);

    return ISTATUS_SUCCESS;
}
