/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    iristoolkit_triangle.h

Abstract:

    This file contains the definitions for the TRIANGLE type.

--*/

#include <iristoolkitp.h>

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
    PTEXTURE Textures[2];
    PNORMAL Normals[2];
    POINT3 Vertex0;
    VECTOR3 B;
    VECTOR3 C;
    VECTOR3 SurfaceNormal;
    VECTOR_AXIS DominantAxis;
} TRIANGLE, *PTRIANGLE;

typedef CONST TRIANGLE *PCTRIANGLE;

//
// Static functions
//

_Check_return_
_Ret_opt_
STATIC
PCTEXTURE 
TriangleGetTexture(
    _In_ PCVOID Context, 
    _In_ UINT32 FaceHit
    )
{
    PCTRIANGLE Triangle;

    ASSERT(Context != NULL);

    Triangle = (PCTRIANGLE) Context;

    if (FaceHit > TRIANGLE_BACK_FACE)
    {
        return NULL;
    }

    return Triangle->Textures[FaceHit];
}

_Check_return_
_Ret_opt_
STATIC
PCNORMAL 
TriangleGetNormal(
    _In_ PCVOID Context, 
    _In_ UINT32 FaceHit
    )
{
    PCTRIANGLE Triangle;

    ASSERT(Context != NULL);

    Triangle = (PCTRIANGLE) Context;

    if (FaceHit > TRIANGLE_BACK_FACE)
    {
        return NULL;
    }

    return Triangle->Normals[FaceHit];
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS 
TriangleTraceTriangle(
    _In_opt_ PCVOID Context, 
    _In_ RAY Ray,
    _Inout_ PSHAPE_HIT_ALLOCATOR ShapeHitAllocator,
    _Outptr_result_maybenull_ PSHAPE_HIT_LIST *ShapeHitList
    )
{
    BARYCENTRIC_COORDINATES BarycentricCoordinates;
    PCTRIANGLE Triangle;
    FLOAT DotProduct;
    FLOAT Distance;
    VECTOR3 Temp;
    POINT3 Hit;
    INT32 Face;
    VECTOR3 P;

    ASSERT(ShapeHitAllocator != NULL);
    ASSERT(ShapeHitList != NULL);
    ASSERT(Context != NULL);

    Triangle = (PCTRIANGLE) Context;

    Temp = PointSubtract(Ray.Origin, Triangle->Vertex0);

    DotProduct = VectorDotProduct(Ray.Direction, Triangle->SurfaceNormal);
    Distance = VectorDotProduct(Temp, Triangle->SurfaceNormal) / -DotProduct;

    if (IsNormalFloat(Distance) == FALSE ||
        IsFiniteFloat(Distance) == FALSE)
    {
        *ShapeHitList = NULL;
        return ISTATUS_SUCCESS;
    }

#if !defined(ENABLE_CSG_SUPPORT)

    if (Distance < (FLOAT) 0.0)
    {
        *ShapeHitList = NULL;
        return ISTATUS_SUCCESS;
    }

#endif // !defined(ENABLE_CSG_SUPPORT)

    Hit = RayEndpoint(Ray, Distance);
    P = PointSubtract(Hit, Triangle->Vertex0);

    switch (Triangle->DominantAxis)
    {
        default:
            ASSERT(FALSE);
        case VECTOR_X_AXIS:
            BarycentricCoordinates.Coordinates[1] = (P.Z * Triangle->C.Y - 
                                                     P.Y * Triangle->C.Z) /
                                                    (Triangle->B.Z * Triangle->C.Y - 
                                                     Triangle->B.Y * Triangle->C.Z);

            BarycentricCoordinates.Coordinates[2] = (P.Z * Triangle->B.Y - 
                                                     P.Y * Triangle->B.Z) /
                                                    (Triangle->C.Z * Triangle->B.Y - 
                                                     Triangle->C.Y * Triangle->B.Z);
            break;
        case VECTOR_Y_AXIS:
            BarycentricCoordinates.Coordinates[1] = (P.X * Triangle->C.Z - 
                                                     P.Z * Triangle->C.X) /
                                                    (Triangle->B.X * Triangle->C.Z - 
                                                     Triangle->B.Z * Triangle->C.X);

            BarycentricCoordinates.Coordinates[2] = (P.X * Triangle->B.Z - 
                                                     P.Z * Triangle->B.X) /
                                                    (Triangle->C.X * Triangle->B.Z - 
                                                     Triangle->C.Z * Triangle->B.X);
            break;
        case VECTOR_Z_AXIS:
            BarycentricCoordinates.Coordinates[1] = (P.Y * Triangle->C.X - 
                                                     P.X * Triangle->C.Y) /
                                                    (Triangle->B.Y * Triangle->C.X - 
                                                     Triangle->B.X * Triangle->C.Y);

            BarycentricCoordinates.Coordinates[2] = (P.Y * Triangle->B.X - 
                                                     P.X * Triangle->B.Y) /
                                                    (Triangle->C.Y * Triangle->B.X - 
                                                     Triangle->C.X * Triangle->B.Y);
            break;
    }

    BarycentricCoordinates.Coordinates[0] = (FLOAT) 1.0 - 
                                            BarycentricCoordinates.Coordinates[1] - 
                                            BarycentricCoordinates.Coordinates[2];

    if (BarycentricCoordinates.Coordinates[0] < (FLOAT) 0.0 || 
        BarycentricCoordinates.Coordinates[1] < (FLOAT) 0.0 || 
        BarycentricCoordinates.Coordinates[2] < (FLOAT) 0.0)
    {
        *ShapeHitList = NULL;
        return ISTATUS_SUCCESS;
    }

    Face = ((FLOAT) 0.0 > DotProduct) ? TRIANGLE_FRONT_FACE : TRIANGLE_BACK_FACE;

    *ShapeHitList = ShapeHitAllocatorAllocateWithHitPoint(ShapeHitAllocator,
                                                          NULL,
                                                          Distance,
                                                          Face,
                                                          &BarycentricCoordinates,
                                                          sizeof(BARYCENTRIC_COORDINATES),
                                                          Hit);

    return (*ShapeHitList == NULL) ? ISTATUS_ALLOCATION_FAILED : ISTATUS_SUCCESS;
}

STATIC
TriangleFree(
    _In_ PVOID Context
    )
{
    PTRIANGLE Triangle;

    Triangle = (PTRIANGLE) Context;

    TextureDereference(Triangle->Textures[TRIANGLE_FRONT_FACE]);
    TextureDereference(Triangle->Textures[TRIANGLE_BACK_FACE]);
    NormalDereference(Triangle->Normals[TRIANGLE_FRONT_FACE]);
    NormalDereference(Triangle->Normals[TRIANGLE_BACK_FACE]);
}

//
// Static variables
//

CONST STATIC DRAWING_SHAPE_VTABLE TriangleHeader = {
    { TriangleTraceTriangle, TriangleFree },
    TriangleGetTexture,
    TriangleGetNormal
};

//
// Static helper routines
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS
TriangleInitialize(
    _Out_ PTRIANGLE Triangle,
    _In_ POINT3 Vertex0,
    _In_ POINT3 Vertex1,
    _In_ POINT3 Vertex2,
    _In_opt_ PTEXTURE FrontTexture,
    _In_opt_ PTEXTURE BackTexture,
    _Out_opt_ PVECTOR3 FrontFaceSurfaceNormal 
    )
{
    FLOAT CrossProductLength;
    VECTOR3 CrossProduct;
    VECTOR3 B;
    VECTOR3 C;

    if (FrontTexture == NULL &&
        BackTexture == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT;
    }

    B = PointSubtract(Vertex1, Vertex0);
    C = PointSubtract(Vertex2, Vertex0);

    CrossProduct = VectorCrossProduct(C, B);

    CrossProductLength = VectorLength(CrossProduct);

    if (CrossProductLength <= TRIANGLE_DEGENERATE_THRESHOLD)
    {
        return ISTATUS_INVALID_ARGUMENT;
    }

    Triangle->Vertex0 = Vertex0;
    Triangle->B = B;
    Triangle->C = C;

    Triangle->SurfaceNormal = VectorNormalize(CrossProduct, NULL);
    Triangle->DominantAxis = VectorDominantAxis(Triangle->SurfaceNormal);

    if (FrontFaceSurfaceNormal != NULL)
    {
        *FrontFaceSurfaceNormal = Triangle->SurfaceNormal;
    }

    Triangle->Textures[TRIANGLE_FRONT_FACE] = FrontTexture;
    Triangle->Textures[TRIANGLE_BACK_FACE] = BackTexture;

    return ISTATUS_SUCCESS;
}

//
// Public functions
//

_Check_return_
_Ret_maybenull_
PDRAWING_SHAPE
TriangleAllocate(
    _In_ PCPOINT3 Vertex0,
    _In_ PCPOINT3 Vertex1,
    _In_ PCPOINT3 Vertex2,
    _In_opt_ PTEXTURE FrontTexture,
    _In_opt_ PNORMAL FrontNormal,
    _In_opt_ PTEXTURE BackTexture,
    _In_opt_ PNORMAL BackNormal
    )
{
    PDRAWING_SHAPE DrawingShape;
    TRIANGLE Triangle;
    ISTATUS Status;

    Status = TriangleInitialize(&Triangle,
                                *Vertex0,
                                *Vertex1,
                                *Vertex2,
                                FrontTexture,
                                BackTexture,
                                NULL);

    if (Status != ISTATUS_SUCCESS)
    {
        return NULL;
    }

    Triangle.Normals[TRIANGLE_FRONT_FACE] = FrontNormal;
    Triangle.Normals[TRIANGLE_BACK_FACE] = BackNormal;

    DrawingShape = DrawingShapeAllocate(&TriangleHeader,
                                        &Triangle,
                                        sizeof(TRIANGLE),
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

_Check_return_
_Ret_maybenull_
_Success_(return != NULL)
IRISTOOLKITAPI
PDRAWING_SHAPE
FlatTriangleAllocate(
    _In_ PCPOINT3 Vertex0,
    _In_ PCPOINT3 Vertex1,
    _In_ PCPOINT3 Vertex2,
    _In_opt_ PTEXTURE FrontTexture,
    _In_opt_ PTEXTURE BackTexture,
    _Out_opt_ PNORMAL *FrontNormal,
    _Out_opt_ PNORMAL *BackNormal
    )
{
    PDRAWING_SHAPE DrawingShape;
    PNORMAL AllocatedFrontNormal;
    PNORMAL AllocatedBackNormal;
    VECTOR3 FrontSurfaceNormal;
    TRIANGLE Triangle;
    ISTATUS Status;

    if ((FrontTexture != NULL && FrontNormal == NULL) ||
        (BackTexture != NULL && BackNormal == NULL))
    {
        return NULL;
    }

    Status = TriangleInitialize(&Triangle,
                                *Vertex0,
                                *Vertex1,
                                *Vertex2,
                                FrontTexture,
                                BackTexture,
                                &FrontSurfaceNormal);

    if (Status != ISTATUS_SUCCESS)
    {
        return NULL;
    }

    if (FrontTexture != NULL)
    {
        AllocatedFrontNormal = ConstantNormalAllocate(&FrontSurfaceNormal,
                                                      FALSE);

        if (AllocatedFrontNormal == NULL)
        {
            return NULL;
        }
    }
    else
    {
        AllocatedFrontNormal = NULL;
    }

    if (BackTexture != NULL)
    {
        FrontSurfaceNormal = VectorNegate(FrontSurfaceNormal);

        AllocatedBackNormal = ConstantNormalAllocate(&FrontSurfaceNormal,
                                                     FALSE);

        if (AllocatedBackNormal == NULL)
        {
            NormalDereference(AllocatedFrontNormal);
            return NULL;
        }
    }
    else
    {
        AllocatedBackNormal = NULL;
    }

    Triangle.Normals[TRIANGLE_FRONT_FACE] = AllocatedFrontNormal;
    Triangle.Normals[TRIANGLE_BACK_FACE] = AllocatedBackNormal;

    if (FrontNormal != NULL)
    {
        *FrontNormal = AllocatedFrontNormal;
    }

    if (BackNormal != NULL)
    {
        *BackNormal = AllocatedBackNormal;
    }

    DrawingShape = DrawingShapeAllocate(&TriangleHeader,
                                        &Triangle,
                                        sizeof(TRIANGLE),
                                        sizeof(PVOID));

    if (DrawingShape != NULL)
    {
        TextureReference(FrontTexture);
        TextureReference(BackTexture);
        NormalReference(*FrontNormal);
        NormalReference(*BackNormal);   
    }
    else
    {
        NormalDereference(AllocatedFrontNormal);
        NormalDereference(AllocatedBackNormal);
    }

    return DrawingShape;
}