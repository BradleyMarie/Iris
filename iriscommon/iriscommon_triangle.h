/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    iriscommon_triangle.h

Abstract:

    This file contains the function definitions for the triangle routines.

--*/

#ifndef _TRIANGLE_IRIS_COMMON_
#define _TRIANGLE_IRIS_COMMON_

#include <irisadvanced.h>

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

typedef struct _BARYCENTRIC_COORDINATES {
    FLOAT Coordinates[3];
} BARYCENTRIC_COORDINATES, *PBARYCENTRIC_COORDINATES;

typedef CONST BARYCENTRIC_COORDINATES *PCBARYCENTRIC_COORDINATES;

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS 
TriangleXDominantTraceTriangle(
    _In_ PCTRIANGLE Triangle, 
    _In_ RAY Ray,
    _Inout_ PSHAPE_HIT_ALLOCATOR ShapeHitAllocator,
    _Outptr_result_maybenull_ PSHAPE_HIT_LIST *ShapeHitList
    )
{
    BARYCENTRIC_COORDINATES BarycentricCoordinates;
    FLOAT Distance;
    FLOAT DotProduct;
    INT32 Face;
    POINT3 Hit;
    VECTOR3 P;
    ISTATUS Status;
    VECTOR3 Temp;

    ASSERT(Triangle != NULL);
    ASSERT(ShapeHitAllocator != NULL);
    ASSERT(ShapeHitList != NULL);

    Temp = PointSubtract(Ray.Origin, Triangle->Vertex0);

    DotProduct = VectorDotProduct(Ray.Direction, Triangle->SurfaceNormal);
    Distance = VectorDotProduct(Temp, Triangle->SurfaceNormal) / -DotProduct;

    if (IsFiniteFloat(Distance) == FALSE)
    {
        *ShapeHitList = NULL;
        return ISTATUS_SUCCESS;
    }

#if !defined(ENABLE_CSG_SUPPORT)

    if (IsGreaterThanOrEqualToZero(Distance) == FALSE)
    {
        *ShapeHitList = NULL;
        return ISTATUS_SUCCESS;
    }

#endif // !defined(ENABLE_CSG_SUPPORT)

    Hit = RayEndpoint(Ray, Distance);
    P = PointSubtract(Hit, Triangle->Vertex0);

    BarycentricCoordinates.Coordinates[1] = (P.Z * Triangle->C.Y - 
                                             P.Y * Triangle->C.Z) /
                                            (Triangle->B.Z * Triangle->C.Y - 
                                             Triangle->B.Y * Triangle->C.Z);

    if (GreaterThanOrEqualToZeroFloat(BarycentricCoordinates.Coordinates[0]) == FALSE)
    {
        *ShapeHitList = NULL;
        return ISTATUS_SUCCESS;
    }

    BarycentricCoordinates.Coordinates[2] = (P.Z * Triangle->B.Y - 
                                             P.Y * Triangle->B.Z) /
                                            (Triangle->C.Z * Triangle->B.Y - 
                                             Triangle->C.Y * Triangle->B.Z);

    if (GreaterThanOrEqualToZeroFloat(BarycentricCoordinates.Coordinates[1]) == FALSE)
    {
        *ShapeHitList = NULL;
        return ISTATUS_SUCCESS;
    }

    BarycentricCoordinates.Coordinates[0] = (FLOAT) 1.0 - 
                                            BarycentricCoordinates.Coordinates[1] - 
                                            BarycentricCoordinates.Coordinates[2];

    if (GreaterThanOrEqualToZeroFloat(BarycentricCoordinates.Coordinates[2]) == FALSE)
    {
        *ShapeHitList = NULL;
        return ISTATUS_SUCCESS;
    }

    Face = ((FLOAT) 0.0 > DotProduct) ? TRIANGLE_FRONT_FACE : TRIANGLE_BACK_FACE;

    Status = ShapeHitAllocatorAllocateWithHitPoint(ShapeHitAllocator,
                                                   NULL,
                                                   Distance,
                                                   Face,
                                                   &BarycentricCoordinates,
                                                   sizeof(BARYCENTRIC_COORDINATES),
                                                   sizeof(FLOAT),
                                                   Hit,
                                                   ShapeHitList);

    return Status;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS 
TriangleYDominantTraceTriangle(
    _In_ PCTRIANGLE Triangle, 
    _In_ RAY Ray,
    _Inout_ PSHAPE_HIT_ALLOCATOR ShapeHitAllocator,
    _Outptr_result_maybenull_ PSHAPE_HIT_LIST *ShapeHitList
    )
{
    BARYCENTRIC_COORDINATES BarycentricCoordinates;
    FLOAT Distance;
    FLOAT DotProduct;
    INT32 Face;
    POINT3 Hit;
    VECTOR3 P;
    ISTATUS Status;
    VECTOR3 Temp;

    ASSERT(Triangle != NULL);
    ASSERT(ShapeHitAllocator != NULL);
    ASSERT(ShapeHitList != NULL);

    Temp = PointSubtract(Ray.Origin, Triangle->Vertex0);

    DotProduct = VectorDotProduct(Ray.Direction, Triangle->SurfaceNormal);
    Distance = VectorDotProduct(Temp, Triangle->SurfaceNormal) / -DotProduct;

    if (IsFiniteFloat(Distance) == FALSE)
    {
        *ShapeHitList = NULL;
        return ISTATUS_SUCCESS;
    }

#if !defined(ENABLE_CSG_SUPPORT)

    if (IsGreaterThanOrEqualToZero(Distance) == FALSE)
    {
        *ShapeHitList = NULL;
        return ISTATUS_SUCCESS;
    }

#endif // !defined(ENABLE_CSG_SUPPORT)

    Hit = RayEndpoint(Ray, Distance);
    P = PointSubtract(Hit, Triangle->Vertex0);

    BarycentricCoordinates.Coordinates[1] = (P.X * Triangle->C.Z - 
                                             P.Z * Triangle->C.X) /
                                            (Triangle->B.X * Triangle->C.Z - 
                                             Triangle->B.Z * Triangle->C.X);

    if (GreaterThanOrEqualToZeroFloat(BarycentricCoordinates.Coordinates[0]) == FALSE)
    {
        *ShapeHitList = NULL;
        return ISTATUS_SUCCESS;
    }

    BarycentricCoordinates.Coordinates[2] = (P.X * Triangle->B.Z - 
                                             P.Z * Triangle->B.X) /
                                            (Triangle->C.X * Triangle->B.Z - 
                                             Triangle->C.Z * Triangle->B.X);

    if (GreaterThanOrEqualToZeroFloat(BarycentricCoordinates.Coordinates[1]) == FALSE)
    {
        *ShapeHitList = NULL;
        return ISTATUS_SUCCESS;
    }

    BarycentricCoordinates.Coordinates[0] = (FLOAT) 1.0 - 
                                            BarycentricCoordinates.Coordinates[1] - 
                                            BarycentricCoordinates.Coordinates[2];


    if (GreaterThanOrEqualToZeroFloat(BarycentricCoordinates.Coordinates[2]) == FALSE)
    {
        *ShapeHitList = NULL;
        return ISTATUS_SUCCESS;
    }
    Face = ((FLOAT) 0.0 > DotProduct) ? TRIANGLE_FRONT_FACE : TRIANGLE_BACK_FACE;

    Status = ShapeHitAllocatorAllocateWithHitPoint(ShapeHitAllocator,
                                                   NULL,
                                                   Distance,
                                                   Face,
                                                   &BarycentricCoordinates,
                                                   sizeof(BARYCENTRIC_COORDINATES),
                                                   sizeof(FLOAT),
                                                   Hit,
                                                   ShapeHitList);

    return Status;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS 
TriangleZDominantTraceTriangle(
    _In_ PCTRIANGLE Triangle, 
    _In_ RAY Ray,
    _Inout_ PSHAPE_HIT_ALLOCATOR ShapeHitAllocator,
    _Outptr_result_maybenull_ PSHAPE_HIT_LIST *ShapeHitList
    )
{
    BARYCENTRIC_COORDINATES BarycentricCoordinates;
    FLOAT Distance;
    FLOAT DotProduct;
    INT32 Face;
    POINT3 Hit;
    VECTOR3 P;
    ISTATUS Status;
    VECTOR3 Temp;

    ASSERT(Triangle != NULL);
    ASSERT(ShapeHitAllocator != NULL);
    ASSERT(ShapeHitList != NULL);

    Temp = PointSubtract(Ray.Origin, Triangle->Vertex0);

    DotProduct = VectorDotProduct(Ray.Direction, Triangle->SurfaceNormal);
    Distance = VectorDotProduct(Temp, Triangle->SurfaceNormal) / -DotProduct;

    if (IsFiniteFloat(Distance) == FALSE)
    {
        *ShapeHitList = NULL;
        return ISTATUS_SUCCESS;
    }

#if !defined(ENABLE_CSG_SUPPORT)

    if (IsGreaterThanOrEqualToZero(Distance) == FALSE)
    {
        *ShapeHitList = NULL;
        return ISTATUS_SUCCESS;
    }

#endif // !defined(ENABLE_CSG_SUPPORT)

    Hit = RayEndpoint(Ray, Distance);
    P = PointSubtract(Hit, Triangle->Vertex0);

    BarycentricCoordinates.Coordinates[1] = (P.Y * Triangle->C.X - 
                                             P.X * Triangle->C.Y) /
                                            (Triangle->B.Y * Triangle->C.X - 
                                             Triangle->B.X * Triangle->C.Y);

    if (GreaterThanOrEqualToZeroFloat(BarycentricCoordinates.Coordinates[0]) == FALSE)
    {
        *ShapeHitList = NULL;
        return ISTATUS_SUCCESS;
    }

    BarycentricCoordinates.Coordinates[2] = (P.Y * Triangle->B.X - 
                                             P.X * Triangle->B.Y) /
                                            (Triangle->C.Y * Triangle->B.X - 
                                             Triangle->C.X * Triangle->B.Y);

    if (GreaterThanOrEqualToZeroFloat(BarycentricCoordinates.Coordinates[1]) == FALSE)
    {
        *ShapeHitList = NULL;
        return ISTATUS_SUCCESS;
    }

    BarycentricCoordinates.Coordinates[0] = (FLOAT) 1.0 - 
                                            BarycentricCoordinates.Coordinates[1] - 
                                            BarycentricCoordinates.Coordinates[2];

    if (GreaterThanOrEqualToZeroFloat(BarycentricCoordinates.Coordinates[2]) == FALSE)
    {
        *ShapeHitList = NULL;
        return ISTATUS_SUCCESS;
    }

    Face = ((FLOAT) 0.0 > DotProduct) ? TRIANGLE_FRONT_FACE : TRIANGLE_BACK_FACE;

    Status = ShapeHitAllocatorAllocateWithHitPoint(ShapeHitAllocator,
                                                   NULL,
                                                   Distance,
                                                   Face,
                                                   &BarycentricCoordinates,
                                                   sizeof(BARYCENTRIC_COORDINATES),
                                                   sizeof(FLOAT),
                                                   Hit,
                                                   ShapeHitList);

    return Status;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
TriangleComputeNormal(
    _In_ PCTRIANGLE Triangle, 
    _In_ POINT3 ModelHitPoint,
    _In_ UINT32 FaceHit,
    _Out_ PVECTOR3 SurfaceNormal
    )
{
    ASSERT(Triangle != NULL);
    ASSERT(PointValidate(ModelHitPoint) != FALSE);
    ASSERT(FaceHit == TRIANGLE_FRONT_FACE || FaceHit == TRIANGLE_BACK_FACE);
    ASSERT(SurfaceNormal != NULL);

    if (FaceHit == TRIANGLE_FRONT_FACE)
    {
        *SurfaceNormal = Triangle->SurfaceNormal;
    }
    else
    {
        *SurfaceNormal = VectorNegate(Triangle->SurfaceNormal);
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

    TransformedVertex0 = VectorMatrixMultiply(ModelToWorld, Triangle->Vertex0);

    if (BoundingBoxContainsPoint(WorldAlignedBoundingBox, TransformedVertex0) != FALSE)
    {
        *IsInsideBox = TRUE;
        return ISTATUS_SUCCESS;
    }

    TransformedVertex1 = PointAdd(Triangle->Vertex0, Triangle->B);
    TransformedVertex1 = VectorMatrixMultiply(ModelToWorld, TransformedVertex1);

    if (BoundingBoxContainsPoint(WorldAlignedBoundingBox, TransformedVertex1) != FALSE)
    {
        *IsInsideBox = TRUE;
        return ISTATUS_SUCCESS;
    }

    TransformedVertex2 = PointAdd(Triangle->Vertex0, Triangle->C);
    TransformedVertex2 = VectorMatrixMultiply(ModelToWorld, TransformedVertex2);

    if (BoundingBoxContainsPoint(WorldAlignedBoundingBox, TransformedVertex2) != FALSE)
    {
        *IsInsideBox = TRUE;
        return ISTATUS_SUCCESS;
    }

    *IsInsideBox = FALSE;
    return ISTATUS_SUCCESS;
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
    _Out_ PVECTOR_AXIS DominantAxis,
    _Out_opt_ PVECTOR3 FrontSurfaceNormal
    )
{
    VECTOR3 B;
    VECTOR3 C;
    VECTOR3 CrossProduct;
    FLOAT CrossProductLength;

    ASSERT(PointValidate(Vertex0) != FALSE);
    ASSERT(PointValidate(Vertex1) != FALSE);
    ASSERT(PointValidate(Vertex2) != FALSE);
    ASSERT(DominantAxis != NULL);
    ASSERT(Triangle != NULL);

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
    
    *DominantAxis = VectorDominantAxis(Triangle->SurfaceNormal);

    if (FrontSurfaceNormal != NULL)
    {
        *FrontSurfaceNormal = VectorNormalize(CrossProduct, NULL, NULL);
    }

    return ISTATUS_SUCCESS;
}

#endif // _TRIANGLE_IRIS_COMMON_