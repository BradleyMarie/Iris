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
    DRAWING_SHAPE ShapeHeader;
    PCSHADER Shaders[2];
    PCNORMAL Normals[2];
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
_Ret_maybenull_
STATIC
PCSHADER 
TriangleGetShader(
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

    return Triangle->Shaders[FaceHit];
}

_Check_return_
_Ret_maybenull_
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
    _In_ PCVOID Context, 
    _In_ PCRAY Ray,
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
    ASSERT(Ray != NULL);

    Triangle = (PCTRIANGLE) Context;

    PointSubtract(&Ray->Origin, &Triangle->Vertex0, &Temp);

    DotProduct = VectorDotProduct(&Ray->Direction, &Triangle->SurfaceNormal);
    Distance = VectorDotProduct(&Temp, &Triangle->SurfaceNormal) / -DotProduct;

#if !defined(ENABLE_CSG_SUPPORT)

    if (Distance < (FLOAT) 0.0)
    {
        *ShapeHitList = NULL;
        return ISTATUS_SUCCESS;
    }

#endif // !defined(ENABLE_CSG_SUPPORT)

    RayEndpoint(Ray, Distance, &Hit);
    PointSubtract(&Hit, &Triangle->Vertex0, &P);

    switch (Triangle->DominantAxis)
    {
        default:
            ASSERT(FALSE);
        case VECTOR_X_AXIS:
            BarycentricCoordinates.B1 = (P.Z * Triangle->C.Y - 
                                         P.Y * Triangle->C.Z) /
                                        (Triangle->B.Z * Triangle->C.Y - 
                                         Triangle->B.Y * Triangle->C.Z);

            BarycentricCoordinates.B2 = (P.Z * Triangle->B.Y - 
                                         P.Y * Triangle->B.Z) /
                                        (Triangle->C.Z * Triangle->B.Y - 
                                         Triangle->C.Y * Triangle->B.Z);
            break;
        case VECTOR_Y_AXIS:
            BarycentricCoordinates.B1 = (P.X * Triangle->C.Z - 
                                         P.Z * Triangle->C.X) /
                                        (Triangle->B.X * Triangle->C.Z - 
                                         Triangle->B.Z * Triangle->C.X);

            BarycentricCoordinates.B2 = (P.X * Triangle->B.Z - 
                                         P.Z * Triangle->B.X) /
                                        (Triangle->C.X * Triangle->B.Z - 
                                         Triangle->C.Z * Triangle->B.X);
            break;
        case VECTOR_Z_AXIS:
            BarycentricCoordinates.B1 = (P.Y * Triangle->C.X - 
                                         P.X * Triangle->C.Y) /
                                        (Triangle->B.Y * Triangle->C.X - 
                                         Triangle->B.X * Triangle->C.Y);

            BarycentricCoordinates.B2 = (P.Y * Triangle->B.X - 
                                         P.X * Triangle->B.Y) /
                                        (Triangle->C.Y * Triangle->B.X - 
                                         Triangle->C.X * Triangle->B.Y);
            break;
    }

    BarycentricCoordinates.B0 = (FLOAT) 1.0 - 
                                BarycentricCoordinates.B1 - 
                                BarycentricCoordinates.B2;

    if (BarycentricCoordinates.B0 < (FLOAT) 0.0 || 
        BarycentricCoordinates.B1 < (FLOAT) 0.0 || 
        BarycentricCoordinates.B2 < (FLOAT) 0.0)
    {
        *ShapeHitList = NULL;
        return ISTATUS_SUCCESS;
    }

    Face = ((FLOAT) 0.0 > DotProduct) ? TRIANGLE_FRONT_FACE : TRIANGLE_BACK_FACE;

    *ShapeHitList = ShapeHitAllocatorAllocate(ShapeHitAllocator,
                                              NULL,
                                              (PCSHAPE) Context,
                                              Distance,
                                              Face,
                                              &BarycentricCoordinates,
                                              sizeof(BARYCENTRIC_COORDINATES));

    return (*ShapeHitList == NULL) ? ISTATUS_ALLOCATION_FAILED : ISTATUS_SUCCESS;
}

//
// Static variables
//

STATIC DRAWING_SHAPE_VTABLE TriangleHeader = {
    { TriangleTraceTriangle },
    free,
    TriangleGetShader,
    TriangleGetNormal
};

//
// Public Functions
//

_Check_return_
_Ret_maybenull_
PDRAWING_SHAPE
TriangleAllocate(
    _In_ PCPOINT3 Vertex0,
    _In_ PCPOINT3 Vertex1,
    _In_ PCPOINT3 Vertex2,
    _In_opt_ PCSHADER FrontShader,
    _In_opt_ PCNORMAL FrontNormal,
    _In_opt_ PCSHADER BackShader,
    _In_opt_ PCNORMAL BackNormal,
    _Out_opt_ PVECTOR3 FrontFaceSurfaceNormal 
    )
{
    FLOAT CrossProductLength;
    VECTOR3 CrossProduct;
    PTRIANGLE Triangle;
    VECTOR3 B;
    VECTOR3 C;

    ASSERT(Vertex0 != NULL);
    ASSERT(Vertex1 != NULL);
    ASSERT(Vertex2 != NULL);

    if (FrontShader == NULL &&
        FrontNormal != NULL)
    {
        return NULL;
    }

    if (BackShader == NULL &&
        BackNormal != NULL)
    {
        return NULL;
    }

    if (FrontShader == NULL &&
        BackShader == NULL)
    {
        return NULL;
    }

    PointSubtract(Vertex1, Vertex0, &B);
    PointSubtract(Vertex2, Vertex0, &C);

    VectorCrossProduct(&C, &B, &CrossProduct);

    CrossProductLength = VectorLength(&CrossProduct);

    if (CrossProductLength <= TRIANGLE_DEGENERATE_THRESHOLD)
    {
        return NULL;
    }

    Triangle = (PTRIANGLE) malloc(sizeof(TRIANGLE));

    if (Triangle == NULL)
    {
        return NULL;
    }

    Triangle->ShapeHeader.DrawingShapeVTable = &TriangleHeader;

    Triangle->Vertex0 = *Vertex0;
    Triangle->B = B;
    Triangle->C = C;

    VectorNormalize(&CrossProduct, &Triangle->SurfaceNormal);
    Triangle->DominantAxis = VectorDominantAxis(&Triangle->SurfaceNormal);

    if (FrontFaceSurfaceNormal != NULL)
    {
        *FrontFaceSurfaceNormal = Triangle->SurfaceNormal;
    }

    Triangle->Shaders[0] = FrontShader;
    Triangle->Shaders[1] = BackShader;
    Triangle->Normals[0] = FrontNormal;
    Triangle->Normals[1] = BackNormal;

    return (PDRAWING_SHAPE) Triangle;
}