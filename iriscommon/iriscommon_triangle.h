/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    iriscommon_triangle.h

Abstract:

    This file contains the function definitions for the triangle routines.

--*/

#ifndef _TRIANGLE_IRIS_COMMON_
#define _TRIANGLE_IRIS_COMMON_

#include <iris.h>

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
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS 
TriangleXDominantTraceTriangle(
    _In_ PCTRIANGLE Triangle, 
    _In_ RAY Ray,
    _Inout_ PSHAPE_HIT_ALLOCATOR ShapeHitAllocator,
    _Outptr_result_maybenull_ PSHAPE_HIT_LIST *ShapeHitList
    )
{
    FLOAT BarycentricCoordinates[3];
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

    if (IsGreaterThanOrEqualToZeroFloat(Distance) == FALSE)
    {
        *ShapeHitList = NULL;
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
        *ShapeHitList = NULL;
        return ISTATUS_SUCCESS;
    }

    BarycentricCoordinates[2] = (P.Z * Triangle->B.Y - 
                                P.Y * Triangle->B.Z) /
                                (Triangle->C.Z * Triangle->B.Y - 
                                Triangle->C.Y * Triangle->B.Z);

    if (IsGreaterThanOrEqualToZeroFloat(BarycentricCoordinates[2]) == FALSE)
    {
        *ShapeHitList = NULL;
        return ISTATUS_SUCCESS;
    }

    BarycentricCoordinates[0] = (FLOAT) 1.0 - 
                                BarycentricCoordinates[1] - 
                                BarycentricCoordinates[2];

    if (IsGreaterThanOrEqualToZeroFloat(BarycentricCoordinates[0]) == FALSE)
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
                                                   sizeof(BarycentricCoordinates),
                                                   _Alignof(FLOAT),
                                                   Hit,
                                                   ShapeHitList);

    return Status;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS 
TriangleYDominantTraceTriangle(
    _In_ PCTRIANGLE Triangle, 
    _In_ RAY Ray,
    _Inout_ PSHAPE_HIT_ALLOCATOR ShapeHitAllocator,
    _Outptr_result_maybenull_ PSHAPE_HIT_LIST *ShapeHitList
    )
{
    FLOAT BarycentricCoordinates[3];
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

    if (IsGreaterThanOrEqualToZeroFloat(Distance) == FALSE)
    {
        *ShapeHitList = NULL;
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
        *ShapeHitList = NULL;
        return ISTATUS_SUCCESS;
    }

    BarycentricCoordinates[2] = (P.X * Triangle->B.Z - 
                                P.Z * Triangle->B.X) /
                                (Triangle->C.X * Triangle->B.Z - 
                                Triangle->C.Z * Triangle->B.X);

    if (IsGreaterThanOrEqualToZeroFloat(BarycentricCoordinates[2]) == FALSE)
    {
        *ShapeHitList = NULL;
        return ISTATUS_SUCCESS;
    }

    BarycentricCoordinates[0] = (FLOAT) 1.0 - 
                                BarycentricCoordinates[1] - 
                                BarycentricCoordinates[2];

    if (IsGreaterThanOrEqualToZeroFloat(BarycentricCoordinates[0]) == FALSE)
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
                                                   sizeof(BarycentricCoordinates),
                                                   _Alignof(FLOAT),
                                                   Hit,
                                                   ShapeHitList);

    return Status;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS 
TriangleZDominantTraceTriangle(
    _In_ PCTRIANGLE Triangle, 
    _In_ RAY Ray,
    _Inout_ PSHAPE_HIT_ALLOCATOR ShapeHitAllocator,
    _Outptr_result_maybenull_ PSHAPE_HIT_LIST *ShapeHitList
    )
{
    FLOAT BarycentricCoordinates[3];
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

    if (IsGreaterThanOrEqualToZeroFloat(Distance) == FALSE)
    {
        *ShapeHitList = NULL;
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
        *ShapeHitList = NULL;
        return ISTATUS_SUCCESS;
    }

    BarycentricCoordinates[2] = (P.Y * Triangle->B.X - 
                                P.X * Triangle->B.Y) /
                                (Triangle->C.Y * Triangle->B.X - 
                                Triangle->C.X * Triangle->B.Y);

    if (IsGreaterThanOrEqualToZeroFloat(BarycentricCoordinates[2]) == FALSE)
    {
        *ShapeHitList = NULL;
        return ISTATUS_SUCCESS;
    }

    BarycentricCoordinates[0] = (FLOAT) 1.0 - 
                                BarycentricCoordinates[1] - 
                                BarycentricCoordinates[2];

    if (IsGreaterThanOrEqualToZeroFloat(BarycentricCoordinates[0]) == FALSE)
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
                                                   sizeof(BarycentricCoordinates),
                                                   _Alignof(FLOAT),
                                                   Hit,
                                                   ShapeHitList);

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

#endif // _TRIANGLE_IRIS_COMMON_