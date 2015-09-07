/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    iriscommon_trianglescene.h

Abstract:

    This file contains the definitions for the TRIANGLE_SCENE type.

--*/

#ifndef _IRIS_COMMON_TRIANGLE_SCENE_
#define _IRIS_COMMON_TRIANGLE_SCENE_

#include <iris.h>

//
// Constants
//

#define MAXIMUM_TRIANGLES_PER_LIST ((SIZE_T) (1 << 29) - 1)
#define TRIANGLE_SCENE_INTIAL_SIZE  16
#define TRIANGLE_BATCH_ALIGNMENT 16
#define TRIANGLES_PER_BATCH 1
#define TRIANGLE_DEGENERATE_THRESHOLD (FLOAT) 0.001
#define TRIANGLE_FRONT_FACE 0
#define TRIANGLE_BACK_FACE  1

//
// Types
//

typedef struct _TRIANGLE_BATCH {
    FLOAT Vertex0X[TRIANGLES_PER_BATCH];
    FLOAT Vertex0Y[TRIANGLES_PER_BATCH];
    FLOAT Vertex0Z[TRIANGLES_PER_BATCH];
    FLOAT BX[TRIANGLES_PER_BATCH];
    FLOAT BY[TRIANGLES_PER_BATCH];
    FLOAT BZ[TRIANGLES_PER_BATCH];
    FLOAT CX[TRIANGLES_PER_BATCH];
    FLOAT CY[TRIANGLES_PER_BATCH];
    FLOAT CZ[TRIANGLES_PER_BATCH];
    FLOAT NormalX[TRIANGLES_PER_BATCH];
    FLOAT NormalY[TRIANGLES_PER_BATCH];
    FLOAT NormalZ[TRIANGLES_PER_BATCH];
} TRIANGLE_BATCH, *PTRIANGLE_BATCH;

typedef CONST TRIANGLE_BATCH *PCTRIANGLE_BATCH;

typedef struct _TRIANGLE_SCENE {
    PTRIANGLE_BATCH XDominatedTriangles;
    SIZE_T XDominatedTrianglesCapacity;
    SIZE_T XDominatedTrianglesSize;
    PTRIANGLE_BATCH YDominatedTriangles;
    SIZE_T YDominatedTrianglesCapacity;
    SIZE_T YDominatedTrianglesSize;
    PTRIANGLE_BATCH ZDominatedTriangles;
    SIZE_T ZDominatedTrianglesCapacity;
    SIZE_T ZDominatedTrianglesSize;
} TRIANGLE_SCENE, *PTRIANGLE_SCENE;

typedef CONST TRIANGLE_SCENE *PCTRIANGLE_SCENE;

typedef union _TRIANGLE_FACE {
    struct {
        UINT32 Face : 1;
        UINT32 DominantAxis : 2;
        UINT32 Index 29;
    } BitFields;
    UINT32 AsUINT32;
} TRIANGLE_FACE, *PTRIANGLE_FACE;

typedef CONST TRIANGLE_FACE *PCTRIANGLE_FACE;

//
// Definitions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS 
TriangleSceneTraceXDominatedTriangles(
    _In_opt_ PCTRIANGLE_SCENE TriangleScene, 
    _In_ RAY Ray,
    _Inout_ PSHAPE_HIT_ALLOCATOR ShapeHitAllocator,
    _Outptr_result_maybenull_ PSHAPE_HIT_LIST *ShapeHitList
    )
{
    SIZE_T BatchIndex;
    BARYCENTRIC_COORDINATES BarycentricCoordinates;
    FLOAT Distance;
    FLOAT DotProduct;
    INT32 Face;
    FLOAT HitX;
    FLOAT HitY;
    FLOAT HitZ;
    SIZE_T LastBatchSize;
    SIZE_T NumberOfBatches;
    ISTATUS Status;
    FLOAT ToPlaneX;
    FLOAT ToPlaneY;
    FLOAT ToPlaneZ;
    TRIANGLE_FACE TriangleFace;
    SIZE_T TriangleIndex;
    FLOAT VertexToHitX;
    FLOAT VertexToHitY;
    FLOAT VertexToHitZ;

    ASSERT(ShapeHitAllocator != NULL);
    ASSERT(ShapeHitList != NULL);
    ASSERT(TriangleScene != NULL);

    *ShapeHitList = NULL;

    NumberOfBatches = TriangleScene->XDominatedTrianglesSize / TRIANGLES_PER_BATCH;

    for (BatchIndex = 0; BatchIndex < NumberOfBatches; BatchIndex++)
    {
        for (TriangleIndex = 0; TriangleIndex < TRIANGLES_PER_BATCH; TriangleIndex++)
        {
            ToPlaneX = Ray.Origin.X - TriangleScene->XDominatedTriangles[BatchIndex].Vertex0X[TriangleIndex];
            ToPlaneY = Ray.Origin.Y - TriangleScene->XDominatedTriangles[BatchIndex].Vertex0Y[TriangleIndex];
            ToPlaneZ = Ray.Origin.Z - TriangleScene->XDominatedTriangles[BatchIndex].Vertex0Z[TriangleIndex];

            DotProduct = Ray.Direction.X * TriangleScene->XDominatedTriangles[BatchIndex].NormalX[TriangleIndex] +
                         Ray.Direction.Y * TriangleScene->XDominatedTriangles[BatchIndex].NormalY[TriangleIndex] +
                         Ray.Direction.Z * TriangleScene->XDominatedTriangles[BatchIndex].NormalZ[TriangleIndex];

            Distance = ToPlaneX * TriangleScene->XDominatedTriangles[BatchIndex].NormalX[TriangleIndex] +
                       ToPlaneY * TriangleScene->XDominatedTriangles[BatchIndex].NormalY[TriangleIndex] +
                       ToPlaneZ * TriangleScene->XDominatedTriangles[BatchIndex].NormalZ[TriangleIndex];

            Distance /= -DotProduct;

            if (IsNormalFloat(Distance) == FALSE ||
                IsFiniteFloat(Distance) == FALSE)
            {
                continue;
            }

    #if !defined(ENABLE_CSG_SUPPORT)

            if (Distance < (FLOAT) 0.0)
            {
                continue;
            }

    #endif // !defined(ENABLE_CSG_SUPPORT)

            HitX = FmaFloat(Ray.Direction.X, Distance, Ray.Origin.X);
            HitY = FmaFloat(Ray.Direction.Y, Distance, Ray.Origin.Y);
            HitZ = FmaFloat(Ray.Direction.Z, Distance, Ray.Origin.Z);

            VertexToHitX = HitX - TriangleScene->XDominatedTriangles[BatchIndex].Vertex0X[TriangleIndex];
            VertexToHitY = HitY - TriangleScene->XDominatedTriangles[BatchIndex].Vertex0Y[TriangleIndex];
            VertexToHitZ = HitZ - TriangleScene->XDominatedTriangles[BatchIndex].Vertex0Z[TriangleIndex];

            BarycentricCoordinates.Coordinates[1] = (VertexToHitZ * TriangleScene->XDominatedTriangles[BatchIndex].CY[TriangleIndex] - 
                                                     VertexToHitY * TriangleScene->XDominatedTriangles[BatchIndex].CZ[TriangleIndex]) /
                                                    (TriangleScene->XDominatedTriangles[BatchIndex].BZ[TriangleIndex] * 
                                                     TriangleScene->XDominatedTriangles[BatchIndex].CY[TriangleIndex] - 
                                                     TriangleScene->XDominatedTriangles[BatchIndex].BY[TriangleIndex] * 
                                                     TriangleScene->XDominatedTriangles[BatchIndex].CZ[TriangleIndex]);

            BarycentricCoordinates.Coordinates[2] = (VertexToHitZ * Triangle->BY[TriangleIndex] - 
                                                     VertexToHitY * Triangle->BZ[TriangleIndex]) /
                                                    (TriangleScene->XDominatedTriangles[BatchIndex].CZ[TriangleIndex] * 
                                                     TriangleScene->XDominatedTriangles[BatchIndex].BY[TriangleIndex] - 
                                                     TriangleScene->XDominatedTriangles[BatchIndex].CY[TriangleIndex] * 
                                                     TriangleScene->XDominatedTriangles[BatchIndex].BZ[TriangleIndex]);

            BarycentricCoordinates.Coordinates[0] = (FLOAT) 1.0 - 
                                                    BarycentricCoordinates.Coordinates[1] - 
                                                    BarycentricCoordinates.Coordinates[2];

            if (BarycentricCoordinates.Coordinates[0] < (FLOAT) 0.0 || 
                BarycentricCoordinates.Coordinates[1] < (FLOAT) 0.0 || 
                BarycentricCoordinates.Coordinates[2] < (FLOAT) 0.0)
            {
                continue;
            }

            Triangle.BitFields.Face = ((FLOAT) 0.0 > DotProduct) ? TRIANGLE_FRONT_FACE : TRIANGLE_BACK_FACE;
            Triangle.BitFields.DominantAxis = VECTOR_X_AXIS;
            Triangle.BitFields.Index = Index;

            Status = ShapeHitAllocatorAllocateWithHitPoint(ShapeHitAllocator,
                                                           NULL,
                                                           Distance,
                                                           Triangle.AsUINT32,
                                                           &BarycentricCoordinates,
                                                           sizeof(BARYCENTRIC_COORDINATES),
                                                           Hit,
                                                           ShapeHitList);

            if (Status != ISTATUS_SUCCESS)
            {
                return Status;
            }
        }
    }

    LastBatchSize = TriangleScene->XDominatedTrianglesSize % TRIANGLES_PER_BATCH;

    for (TriangleIndex = 0; TriangleIndex < LastBatchSize; TriangleIndex++)
    {
        ToPlaneX = Ray.Origin.X - TriangleScene->XDominatedTriangles[BatchIndex].Vertex0X[TriangleIndex];
        ToPlaneY = Ray.Origin.Y - TriangleScene->XDominatedTriangles[BatchIndex].Vertex0Y[TriangleIndex];
        ToPlaneZ = Ray.Origin.Z - TriangleScene->XDominatedTriangles[BatchIndex].Vertex0Z[TriangleIndex];

        DotProduct = Ray.Direction.X * TriangleScene->XDominatedTriangles[BatchIndex].NormalX[TriangleIndex] +
                     Ray.Direction.Y * TriangleScene->XDominatedTriangles[BatchIndex].NormalY[TriangleIndex] +
                     Ray.Direction.Z * TriangleScene->XDominatedTriangles[BatchIndex].NormalZ[TriangleIndex];

        Distance = ToPlaneX * TriangleScene->XDominatedTriangles[BatchIndex].NormalX[TriangleIndex] +
                   ToPlaneY * TriangleScene->XDominatedTriangles[BatchIndex].NormalY[TriangleIndex] +
                   ToPlaneZ * TriangleScene->XDominatedTriangles[BatchIndex].NormalZ[TriangleIndex];

        Distance /= -DotProduct;

        if (IsNormalFloat(Distance) == FALSE ||
            IsFiniteFloat(Distance) == FALSE)
        {
            continue;
        }

#if !defined(ENABLE_CSG_SUPPORT)

        if (Distance < (FLOAT) 0.0)
        {
            continue;
        }

#endif // !defined(ENABLE_CSG_SUPPORT)

        HitX = FmaFloat(Ray.Direction.X, Distance, Ray.Origin.X);
        HitY = FmaFloat(Ray.Direction.Y, Distance, Ray.Origin.Y);
        HitZ = FmaFloat(Ray.Direction.Z, Distance, Ray.Origin.Z);

        VertexToHitX = HitX - TriangleScene->XDominatedTriangles[BatchIndex].Vertex0X[TriangleIndex];
        VertexToHitY = HitY - TriangleScene->XDominatedTriangles[BatchIndex].Vertex0Y[TriangleIndex];
        VertexToHitZ = HitZ - TriangleScene->XDominatedTriangles[BatchIndex].Vertex0Z[TriangleIndex];

        BarycentricCoordinates.Coordinates[1] = (VertexToHitZ * TriangleScene->XDominatedTriangles[BatchIndex].CY[TriangleIndex] - 
                                                 VertexToHitY * TriangleScene->XDominatedTriangles[BatchIndex].CZ[TriangleIndex]) /
                                                (TriangleScene->XDominatedTriangles[BatchIndex].BZ[TriangleIndex] * 
                                                 TriangleScene->XDominatedTriangles[BatchIndex].CY[TriangleIndex] - 
                                                 TriangleScene->XDominatedTriangles[BatchIndex].BY[TriangleIndex] * 
                                                 TriangleScene->XDominatedTriangles[BatchIndex].CZ[TriangleIndex]);

        BarycentricCoordinates.Coordinates[2] = (VertexToHitZ * Triangle->BY[TriangleIndex] - 
                                                 VertexToHitY * Triangle->BZ[TriangleIndex]) /
                                                (TriangleScene->XDominatedTriangles[BatchIndex].CZ[TriangleIndex] * 
                                                 TriangleScene->XDominatedTriangles[BatchIndex].BY[TriangleIndex] - 
                                                 TriangleScene->XDominatedTriangles[BatchIndex].CY[TriangleIndex] * 
                                                 TriangleScene->XDominatedTriangles[BatchIndex].BZ[TriangleIndex]);

        BarycentricCoordinates.Coordinates[0] = (FLOAT) 1.0 - 
                                                BarycentricCoordinates.Coordinates[1] - 
                                                BarycentricCoordinates.Coordinates[2];

        if (BarycentricCoordinates.Coordinates[0] < (FLOAT) 0.0 || 
            BarycentricCoordinates.Coordinates[1] < (FLOAT) 0.0 || 
            BarycentricCoordinates.Coordinates[2] < (FLOAT) 0.0)
        {
            continue;
        }

        Triangle.BitFields.Face = ((FLOAT) 0.0 > DotProduct) ? TRIANGLE_FRONT_FACE : TRIANGLE_BACK_FACE;
        Triangle.BitFields.DominantAxis = VECTOR_X_AXIS;
        Triangle.BitFields.Index = Index;

        Status = ShapeHitAllocatorAllocateWithHitPoint(ShapeHitAllocator,
                                                       NULL,
                                                       Distance,
                                                       Triangle.AsUINT32,
                                                       &BarycentricCoordinates,
                                                       sizeof(BARYCENTRIC_COORDINATES),
                                                       Hit,
                                                       ShapeHitList);

        if (Status != ISTATUS_SUCCESS)
        {
            return Status;
        }
    }

    return Status;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS 
TriangleSceneTraceYDominatedTriangles(
    _In_opt_ PCTRIANGLE_SCENE TriangleScene, 
    _In_ RAY Ray,
    _Inout_ PSHAPE_HIT_ALLOCATOR ShapeHitAllocator,
    _Outptr_result_maybenull_ PSHAPE_HIT_LIST *ShapeHitList
    )
{
    SIZE_T BatchIndex;
    BARYCENTRIC_COORDINATES BarycentricCoordinates;
    FLOAT Distance;
    FLOAT DotProduct;
    INT32 Face;
    FLOAT HitX;
    FLOAT HitY;
    FLOAT HitZ;
    SIZE_T LastBatchSize;
    SIZE_T NumberOfBatches;
    ISTATUS Status;
    FLOAT ToPlaneX;
    FLOAT ToPlaneY;
    FLOAT ToPlaneZ;
    TRIANGLE_FACE TriangleFace;
    SIZE_T TriangleIndex;
    FLOAT VertexToHitX;
    FLOAT VertexToHitY;
    FLOAT VertexToHitZ;

    ASSERT(ShapeHitAllocator != NULL);
    ASSERT(ShapeHitList != NULL);
    ASSERT(TriangleScene != NULL);

    *ShapeHitList = NULL;

    NumberOfBatches = TriangleScene->YDominatedTrianglesSize / TRIANGLES_PER_BATCH;

    for (BatchIndex = 0; BatchIndex < NumberOfBatches; BatchIndex++)
    {
        for (TriangleIndex = 0; TriangleIndex < TRIANGLES_PER_BATCH; TriangleIndex++)
        {
            ToPlaneX = Ray.Origin.X - TriangleScene->YDominatedTriangles[BatchIndex].Vertex0X[TriangleIndex];
            ToPlaneY = Ray.Origin.Y - TriangleScene->YDominatedTriangles[BatchIndex].Vertex0Y[TriangleIndex];
            ToPlaneZ = Ray.Origin.Z - TriangleScene->YDominatedTriangles[BatchIndex].Vertex0Z[TriangleIndex];

            DotProduct = Ray.Direction.X * TriangleScene->YDominatedTriangles[BatchIndex].NormalX[TriangleIndex] +
                         Ray.Direction.Y * TriangleScene->YDominatedTriangles[BatchIndex].NormalY[TriangleIndex] +
                         Ray.Direction.Z * TriangleScene->YDominatedTriangles[BatchIndex].NormalZ[TriangleIndex];

            Distance = ToPlaneX * TriangleScene->YDominatedTriangles[BatchIndex].NormalX[TriangleIndex] +
                       ToPlaneY * TriangleScene->YDominatedTriangles[BatchIndex].NormalY[TriangleIndex] +
                       ToPlaneZ * TriangleScene->YDominatedTriangles[BatchIndex].NormalZ[TriangleIndex];

            Distance /= -DotProduct;

            if (IsNormalFloat(Distance) == FALSE ||
                IsFiniteFloat(Distance) == FALSE)
            {
                continue;
            }

    #if !defined(ENABLE_CSG_SUPPORT)

            if (Distance < (FLOAT) 0.0)
            {
                continue;
            }

    #endif // !defined(ENABLE_CSG_SUPPORT)

            HitX = FmaFloat(Ray.Direction.X, Distance, Ray.Origin.X);
            HitY = FmaFloat(Ray.Direction.Y, Distance, Ray.Origin.Y);
            HitZ = FmaFloat(Ray.Direction.Z, Distance, Ray.Origin.Z);

            VertexToHitX = HitX - TriangleScene->YDominatedTriangles[BatchIndex].Vertex0X[TriangleIndex];
            VertexToHitY = HitY - TriangleScene->YDominatedTriangles[BatchIndex].Vertex0Y[TriangleIndex];
            VertexToHitZ = HitZ - TriangleScene->YDominatedTriangles[BatchIndex].Vertex0Z[TriangleIndex];

            BarycentricCoordinates.Coordinates[1] = (VertexToHitX * TriangleScene->YDominatedTriangles[BatchIndex].CZ[TriangleIndex] - 
                                                     VertexToHitZ * TriangleScene->YDominatedTriangles[BatchIndex].CX[TriangleIndex]) /
                                                    (TriangleScene->YDominatedTriangles[BatchIndex].BX[TriangleIndex] * 
                                                     TriangleScene->YDominatedTriangles[BatchIndex].CZ[TriangleIndex] - 
                                                     TriangleScene->YDominatedTriangles[BatchIndex].BZ[TriangleIndex] *
                                                     TriangleScene->YDominatedTriangles[BatchIndex].CX[TriangleIndex]);

            BarycentricCoordinates.Coordinates[2] = (VertexToHitX * TriangleScene->YDominatedTriangles[BatchIndex].BZ[TriangleIndex] - 
                                                     VertexToHitZ * TriangleScene->YDominatedTriangles[BatchIndex].BX[TriangleIndex]) /
                                                    (TriangleScene->YDominatedTriangles[BatchIndex].CX[TriangleIndex] * 
                                                     TriangleScene->YDominatedTriangles[BatchIndex].BZ[TriangleIndex] - 
                                                     TriangleScene->YDominatedTriangles[BatchIndex].CZ[TriangleIndex] *
                                                     TriangleScene->YDominatedTriangles[BatchIndex].BX[TriangleIndex]);

            BarycentricCoordinates.Coordinates[0] = (FLOAT) 1.0 - 
                                                    BarycentricCoordinates.Coordinates[1] - 
                                                    BarycentricCoordinates.Coordinates[2];

            if (BarycentricCoordinates.Coordinates[0] < (FLOAT) 0.0 || 
                BarycentricCoordinates.Coordinates[1] < (FLOAT) 0.0 || 
                BarycentricCoordinates.Coordinates[2] < (FLOAT) 0.0)
            {
                continue;
            }

            Triangle.BitFields.Face = ((FLOAT) 0.0 > DotProduct) ? TRIANGLE_FRONT_FACE : TRIANGLE_BACK_FACE;
            Triangle.BitFields.DominantAxis = VECTOR_Y_AXIS;
            Triangle.BitFields.Index = Index;

            Status = ShapeHitAllocatorAllocateWithHitPoint(ShapeHitAllocator,
                                                           NULL,
                                                           Distance,
                                                           Triangle.AsUINT32,
                                                           &BarycentricCoordinates,
                                                           sizeof(BARYCENTRIC_COORDINATES),
                                                           Hit,
                                                           ShapeHitList);

            if (Status != ISTATUS_SUCCESS)
            {
                return Status;
            }
        }
    }

    LastBatchSize = TriangleScene->YDominatedTrianglesSize % TRIANGLES_PER_BATCH;

    for (TriangleIndex = 0; TriangleIndex < LastBatchSize; TriangleIndex++)
    {
        ToPlaneX = Ray.Origin.X - TriangleScene->YDominatedTriangles[BatchIndex].Vertex0X[TriangleIndex];
        ToPlaneY = Ray.Origin.Y - TriangleScene->YDominatedTriangles[BatchIndex].Vertex0Y[TriangleIndex];
        ToPlaneZ = Ray.Origin.Z - TriangleScene->YDominatedTriangles[BatchIndex].Vertex0Z[TriangleIndex];

        DotProduct = Ray.Direction.X * TriangleScene->YDominatedTriangles[BatchIndex].NormalX[TriangleIndex] +
                     Ray.Direction.Y * TriangleScene->YDominatedTriangles[BatchIndex].NormalY[TriangleIndex] +
                     Ray.Direction.Z * TriangleScene->YDominatedTriangles[BatchIndex].NormalZ[TriangleIndex];

        Distance = ToPlaneX * TriangleScene->YDominatedTriangles[BatchIndex].NormalX[TriangleIndex] +
                   ToPlaneY * TriangleScene->YDominatedTriangles[BatchIndex].NormalY[TriangleIndex] +
                   ToPlaneZ * TriangleScene->YDominatedTriangles[BatchIndex].NormalZ[TriangleIndex];

        Distance /= -DotProduct;

        if (IsNormalFloat(Distance) == FALSE ||
            IsFiniteFloat(Distance) == FALSE)
        {
            continue;
        }

#if !defined(ENABLE_CSG_SUPPORT)

        if (Distance < (FLOAT) 0.0)
        {
            continue;
        }

#endif // !defined(ENABLE_CSG_SUPPORT)

        HitX = FmaFloat(Ray.Direction.X, Distance, Ray.Origin.X);
        HitY = FmaFloat(Ray.Direction.Y, Distance, Ray.Origin.Y);
        HitZ = FmaFloat(Ray.Direction.Z, Distance, Ray.Origin.Z);

        VertexToHitX = HitX - TriangleScene->YDominatedTriangles[BatchIndex].Vertex0X[TriangleIndex];
        VertexToHitY = HitY - TriangleScene->YDominatedTriangles[BatchIndex].Vertex0Y[TriangleIndex];
        VertexToHitZ = HitZ - TriangleScene->YDominatedTriangles[BatchIndex].Vertex0Z[TriangleIndex];

        BarycentricCoordinates.Coordinates[1] = (VertexToHitX * TriangleScene->YDominatedTriangles[BatchIndex].CZ[TriangleIndex] - 
                                                 VertexToHitZ * TriangleScene->YDominatedTriangles[BatchIndex].CX[TriangleIndex]) /
                                                (TriangleScene->YDominatedTriangles[BatchIndex].BX[TriangleIndex] * 
                                                 TriangleScene->YDominatedTriangles[BatchIndex].CZ[TriangleIndex] - 
                                                 TriangleScene->YDominatedTriangles[BatchIndex].BZ[TriangleIndex] *
                                                 TriangleScene->YDominatedTriangles[BatchIndex].CX[TriangleIndex]);

        BarycentricCoordinates.Coordinates[2] = (VertexToHitX * TriangleScene->YDominatedTriangles[BatchIndex].BZ[TriangleIndex] - 
                                                 VertexToHitZ * TriangleScene->YDominatedTriangles[BatchIndex].BX[TriangleIndex]) /
                                                (TriangleScene->YDominatedTriangles[BatchIndex].CX[TriangleIndex] * 
                                                 TriangleScene->YDominatedTriangles[BatchIndex].BZ[TriangleIndex] - 
                                                 TriangleScene->YDominatedTriangles[BatchIndex].CZ[TriangleIndex] *
                                                 TriangleScene->YDominatedTriangles[BatchIndex].BX[TriangleIndex]);

        BarycentricCoordinates.Coordinates[0] = (FLOAT) 1.0 - 
                                                BarycentricCoordinates.Coordinates[1] - 
                                                BarycentricCoordinates.Coordinates[2];

        if (BarycentricCoordinates.Coordinates[0] < (FLOAT) 0.0 || 
            BarycentricCoordinates.Coordinates[1] < (FLOAT) 0.0 || 
            BarycentricCoordinates.Coordinates[2] < (FLOAT) 0.0)
        {
            continue;
        }

        Triangle.BitFields.Face = ((FLOAT) 0.0 > DotProduct) ? TRIANGLE_FRONT_FACE : TRIANGLE_BACK_FACE;
        Triangle.BitFields.DominantAxis = VECTOR_Y_AXIS;
        Triangle.BitFields.Index = Index;

        Status = ShapeHitAllocatorAllocateWithHitPoint(ShapeHitAllocator,
                                                       NULL,
                                                       Distance,
                                                       Triangle.AsUINT32,
                                                       &BarycentricCoordinates,
                                                       sizeof(BARYCENTRIC_COORDINATES),
                                                       Hit,
                                                       ShapeHitList);

        if (Status != ISTATUS_SUCCESS)
        {
            return Status;
        }
    }

    return Status;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS 
TriangleSceneTraceZDominatedTriangles(
    _In_opt_ PCTRIANGLE_SCENE TriangleScene, 
    _In_ RAY Ray,
    _Inout_ PSHAPE_HIT_ALLOCATOR ShapeHitAllocator,
    _Outptr_result_maybenull_ PSHAPE_HIT_LIST *ShapeHitList
    )
{
    SIZE_T BatchIndex;
    BARYCENTRIC_COORDINATES BarycentricCoordinates;
    FLOAT Distance;
    FLOAT DotProduct;
    INT32 Face;
    FLOAT HitX;
    FLOAT HitY;
    FLOAT HitZ;
    SIZE_T LastBatchSize;
    SIZE_T NumberOfBatches;
    ISTATUS Status;
    FLOAT ToPlaneX;
    FLOAT ToPlaneY;
    FLOAT ToPlaneZ;
    TRIANGLE_FACE TriangleFace;
    SIZE_T TriangleIndex;
    FLOAT VertexToHitX;
    FLOAT VertexToHitY;
    FLOAT VertexToHitZ;

    ASSERT(ShapeHitAllocator != NULL);
    ASSERT(ShapeHitList != NULL);
    ASSERT(TriangleScene != NULL);

    *ShapeHitList = NULL;

    NumberOfBatches = TriangleScene->ZDominatedTrianglesSize / TRIANGLES_PER_BATCH;

    for (BatchIndex = 0; BatchIndex < NumberOfBatches; BatchIndex++)
    {
        for (TriangleIndex = 0; TriangleIndex < TRIANGLES_PER_BATCH; TriangleIndex++)
        {
            ToPlaneX = Ray.Origin.X - TriangleScene->ZDominatedTriangles[BatchIndex].Vertex0X[TriangleIndex];
            ToPlaneY = Ray.Origin.Y - TriangleScene->ZDominatedTriangles[BatchIndex].Vertex0Y[TriangleIndex];
            ToPlaneZ = Ray.Origin.Z - TriangleScene->ZDominatedTriangles[BatchIndex].Vertex0Z[TriangleIndex];

            DotProduct = Ray.Direction.X * TriangleScene->ZDominatedTriangles[BatchIndex].NormalX[TriangleIndex] +
                         Ray.Direction.Y * TriangleScene->ZDominatedTriangles[BatchIndex].NormalY[TriangleIndex] +
                         Ray.Direction.Z * TriangleScene->ZDominatedTriangles[BatchIndex].NormalZ[TriangleIndex];

            Distance = ToPlaneX * TriangleScene->ZDominatedTriangles[BatchIndex].NormalX[TriangleIndex] +
                       ToPlaneY * TriangleScene->ZDominatedTriangles[BatchIndex].NormalY[TriangleIndex] +
                       ToPlaneZ * TriangleScene->ZDominatedTriangles[BatchIndex].NormalZ[TriangleIndex];

            Distance /= -DotProduct;

            if (IsNormalFloat(Distance) == FALSE ||
                IsFiniteFloat(Distance) == FALSE)
            {
                continue;
            }

    #if !defined(ENABLE_CSG_SUPPORT)

            if (Distance < (FLOAT) 0.0)
            {
                continue;
            }

    #endif // !defined(ENABLE_CSG_SUPPORT)

            HitX = FmaFloat(Ray.Direction.X, Distance, Ray.Origin.X);
            HitY = FmaFloat(Ray.Direction.Y, Distance, Ray.Origin.Y);
            HitZ = FmaFloat(Ray.Direction.Z, Distance, Ray.Origin.Z);

            VertexToHitX = HitX - TriangleScene->ZDominatedTriangles[BatchIndex].Vertex0X[TriangleIndex];
            VertexToHitY = HitY - TriangleScene->ZDominatedTriangles[BatchIndex].Vertex0Y[TriangleIndex];
            VertexToHitZ = HitZ - TriangleScene->ZDominatedTriangles[BatchIndex].Vertex0Z[TriangleIndex];

            BarycentricCoordinates.Coordinates[1] = (VertexToHitY * TriangleScene->ZDominatedTriangles[BatchIndex].CX[TriangleIndex] - 
                                                     VertexToHitX * TriangleScene->ZDominatedTriangles[BatchIndex].CY[TriangleIndex]) /
                                                    (TriangleScene->ZDominatedTriangles[BatchIndex].BY[TriangleIndex] * 
                                                     TriangleScene->ZDominatedTriangles[BatchIndex].CX[TriangleIndex] - 
                                                     TriangleScene->ZDominatedTriangles[BatchIndex].BX[TriangleIndex] * 
                                                     TriangleScene->ZDominatedTriangles[BatchIndex].CY[TriangleIndex]);

            BarycentricCoordinates.Coordinates[2] = (VertexToHitY * TriangleScene->ZDominatedTriangles[BatchIndex].BX[TriangleIndex] - 
                                                     VertexToHitX * TriangleScene->ZDominatedTriangles[BatchIndex].BY[TriangleIndex]) /
                                                    (TriangleScene->ZDominatedTriangles[BatchIndex].CY[TriangleIndex] * 
                                                     TriangleScene->ZDominatedTriangles[BatchIndex].BX[TriangleIndex] - 
                                                     TriangleScene->ZDominatedTriangles[BatchIndex].CX[TriangleIndex] * 
                                                     TriangleScene->ZDominatedTriangles[BatchIndex].BY[TriangleIndex]);

            BarycentricCoordinates.Coordinates[0] = (FLOAT) 1.0 - 
                                                    BarycentricCoordinates.Coordinates[1] - 
                                                    BarycentricCoordinates.Coordinates[2];

            if (BarycentricCoordinates.Coordinates[0] < (FLOAT) 0.0 || 
                BarycentricCoordinates.Coordinates[1] < (FLOAT) 0.0 || 
                BarycentricCoordinates.Coordinates[2] < (FLOAT) 0.0)
            {
                continue;
            }

            Triangle.BitFields.Face = ((FLOAT) 0.0 > DotProduct) ? TRIANGLE_FRONT_FACE : TRIANGLE_BACK_FACE;
            Triangle.BitFields.DominantAxis = VECTOR_Z_AXIS;
            Triangle.BitFields.Index = Index;

            Status = ShapeHitAllocatorAllocateWithHitPoint(ShapeHitAllocator,
                                                           NULL,
                                                           Distance,
                                                           TriangleFace.AsUINT32,
                                                           &BarycentricCoordinates,
                                                           sizeof(BARYCENTRIC_COORDINATES),
                                                           Hit,
                                                           ShapeHitList);

            if (Status != ISTATUS_SUCCESS)
            {
                return Status;
            }
        }
    }

    LastBatchSize = TriangleScene->ZDominatedTrianglesSize % TRIANGLES_PER_BATCH;

    for (TriangleIndex = 0; TriangleIndex < LastBatchSize; TriangleIndex++)
    {
        ToPlaneX = Ray.Origin.X - TriangleScene->ZDominatedTriangles[BatchIndex].Vertex0X[TriangleIndex];
        ToPlaneY = Ray.Origin.Y - TriangleScene->ZDominatedTriangles[BatchIndex].Vertex0Y[TriangleIndex];
        ToPlaneZ = Ray.Origin.Z - TriangleScene->ZDominatedTriangles[BatchIndex].Vertex0Z[TriangleIndex];

        DotProduct = Ray.Direction.X * TriangleScene->ZDominatedTriangles[BatchIndex].NormalX[TriangleIndex] +
                     Ray.Direction.Y * TriangleScene->ZDominatedTriangles[BatchIndex].NormalY[TriangleIndex] +
                     Ray.Direction.Z * TriangleScene->ZDominatedTriangles[BatchIndex].NormalZ[TriangleIndex];

        Distance = ToPlaneX * TriangleScene->ZDominatedTriangles[BatchIndex].NormalX[TriangleIndex] +
                   ToPlaneY * TriangleScene->ZDominatedTriangles[BatchIndex].NormalY[TriangleIndex] +
                   ToPlaneZ * TriangleScene->ZDominatedTriangles[BatchIndex].NormalZ[TriangleIndex];

        Distance /= -DotProduct;

        if (IsNormalFloat(Distance) == FALSE ||
            IsFiniteFloat(Distance) == FALSE)
        {
            continue;
        }

#if !defined(ENABLE_CSG_SUPPORT)

        if (Distance < (FLOAT) 0.0)
        {
            continue;
        }

#endif // !defined(ENABLE_CSG_SUPPORT)

        HitX = FmaFloat(Ray.Direction.X, Distance, Ray.Origin.X);
        HitY = FmaFloat(Ray.Direction.Y, Distance, Ray.Origin.Y);
        HitZ = FmaFloat(Ray.Direction.Z, Distance, Ray.Origin.Z);

        VertexToHitX = HitX - TriangleScene->ZDominatedTriangles[BatchIndex].Vertex0X[TriangleIndex];
        VertexToHitY = HitY - TriangleScene->ZDominatedTriangles[BatchIndex].Vertex0Y[TriangleIndex];
        VertexToHitZ = HitZ - TriangleScene->ZDominatedTriangles[BatchIndex].Vertex0Z[TriangleIndex];

        BarycentricCoordinates.Coordinates[1] = (VertexToHitY * TriangleScene->ZDominatedTriangles[BatchIndex].CX[TriangleIndex] - 
                                                 VertexToHitX * TriangleScene->ZDominatedTriangles[BatchIndex].CY[TriangleIndex]) /
                                                (TriangleScene->ZDominatedTriangles[BatchIndex].BY[TriangleIndex] * 
                                                 TriangleScene->ZDominatedTriangles[BatchIndex].CX[TriangleIndex] - 
                                                 TriangleScene->ZDominatedTriangles[BatchIndex].BX[TriangleIndex] * 
                                                 TriangleScene->ZDominatedTriangles[BatchIndex].CY[TriangleIndex]);

        BarycentricCoordinates.Coordinates[2] = (VertexToHitY * TriangleScene->ZDominatedTriangles[BatchIndex].BX[TriangleIndex] - 
                                                 VertexToHitX * TriangleScene->ZDominatedTriangles[BatchIndex].BY[TriangleIndex]) /
                                                (TriangleScene->ZDominatedTriangles[BatchIndex].CY[TriangleIndex] * 
                                                 TriangleScene->ZDominatedTriangles[BatchIndex].BX[TriangleIndex] - 
                                                 TriangleScene->ZDominatedTriangles[BatchIndex].CX[TriangleIndex] * 
                                                 TriangleScene->ZDominatedTriangles[BatchIndex].BY[TriangleIndex]);

        BarycentricCoordinates.Coordinates[0] = (FLOAT) 1.0 - 
                                                BarycentricCoordinates.Coordinates[1] - 
                                                BarycentricCoordinates.Coordinates[2];

        if (BarycentricCoordinates.Coordinates[0] < (FLOAT) 0.0 || 
            BarycentricCoordinates.Coordinates[1] < (FLOAT) 0.0 || 
            BarycentricCoordinates.Coordinates[2] < (FLOAT) 0.0)
        {
            continue;
        }

        Triangle.BitFields.Face = ((FLOAT) 0.0 > DotProduct) ? TRIANGLE_FRONT_FACE : TRIANGLE_BACK_FACE;
        Triangle.BitFields.DominantAxis = VECTOR_Z_AXIS;
        Triangle.BitFields.Index = Index;

        Status = ShapeHitAllocatorAllocateWithHitPoint(ShapeHitAllocator,
                                                       NULL,
                                                       Distance,
                                                       TriangleFace.AsUINT32,
                                                       &BarycentricCoordinates,
                                                       sizeof(BARYCENTRIC_COORDINATES),
                                                       Hit,
                                                       ShapeHitList);

        if (Status != ISTATUS_SUCCESS)
        {
            return Status;
        }
    }

    return Status;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
TriangleSceneInitialize(
    _Out_ PTRIANGLE_SCENE Scene
    )
{
    PTRIANGLE_BATCH XDominatedTriangles;
    PTRIANGLE_BATCH YDominatedTriangles;
    PTRIANGLE_BATCH ZDominatedTriangles;

    ASSERT(Scene != NULL);

    XDominatedTriangles = (PTRIANGLE_BATCH) IrisAlignedMalloc(sizeof(TRIANGLE_BATCH) * TRIANGLE_SCENE_INTIAL_SIZE,
                                                              TRIANGLE_BATCH_ALIGNMENT);

    if (XDominatedTriangles == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    YDominatedTriangles = (PTRIANGLE_BATCH) IrisAlignedMalloc(sizeof(TRIANGLE_BATCH) * TRIANGLE_SCENE_INTIAL_SIZE,
                                                              TRIANGLE_BATCH_ALIGNMENT);

    if (YDominatedTriangles == NULL)
    {
        IrisAlignedFree(XDominatedTriangles);
        return ISTATUS_ALLOCATION_FAILED;
    }

    ZDominatedTriangles = (PTRIANGLE_BATCH) IrisAlignedMalloc(sizeof(TRIANGLE_BATCH) * TRIANGLE_SCENE_INTIAL_SIZE,
                                                              TRIANGLE_BATCH_ALIGNMENT);

    if (ZDominatedTriangles == NULL)
    {
        IrisAlignedFree(XDominatedTriangles);
        IrisAlignedFree(YDominatedTriangles);
        return ISTATUS_ALLOCATION_FAILED;
    }

    Scene->XDominatedTrianglesCapacity = TRIANGLE_SCENE_INTIAL_SIZE;
    Scene->YDominatedTrianglesCapacity = TRIANGLE_SCENE_INTIAL_SIZE;
    Scene->ZDominatedTrianglesCapacity = TRIANGLE_SCENE_INTIAL_SIZE;
    Scene->XDominatedTriangles = XDominatedTriangles;
    Scene->YDominatedTriangles = YDominatedTriangles;
    Scene->ZDominatedTriangles = ZDominatedTriangles;
    Scene->XDominatedTrianglesSize = 0;
    Scene->YDominatedTrianglesSize = 0;
    Scene->ZDominatedTrianglesSize = 0;

    return ISTATUS_SUCCESS;
}

#undef TRIANGLE_SCENE_INTIAL_SIZE

#endif // _IRIS_COMMON_TRIANGLE_SCENE_