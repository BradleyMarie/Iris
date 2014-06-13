/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    infiniteplane.c

Abstract:

    This file contains the definitions for the INFINITE_PLANE type.

--*/

#include <iristoolkitp.h>

//
// Constants
//

#define INFINITE_PLANE_FRONT_FACE 0
#define INFINITE_PLANE_BACK_FACE  1

//
// Types
//

typedef struct _INFINITE_PLANE {
    PCTEXTURE Textures[2];
    PCNORMAL Normals[2];
    POINT3 Vertex;
    VECTOR3 SurfaceNormal;
} INFINITE_PLANE, *PINFINITE_PLANE;

typedef CONST INFINITE_PLANE *PCINFINITE_PLANE;

//
// Static functions
//

_Check_return_
_Ret_opt_
STATIC
PCTEXTURE 
InfinitePlaneGetTexture(
    _In_ PCVOID Context, 
    _In_ UINT32 FaceHit
    )
{
    PCINFINITE_PLANE InfinitePlane;

    ASSERT(Context != NULL);

    InfinitePlane = (PCINFINITE_PLANE) Context;

    if (FaceHit > INFINITE_PLANE_BACK_FACE)
    {
        return NULL;
    }

    return InfinitePlane->Textures[FaceHit];
}

_Check_return_
_Ret_opt_
STATIC
PCNORMAL 
InfinitePlaneGetNormal(
    _In_ PCVOID Context, 
    _In_ UINT32 FaceHit
    )
{
    PCINFINITE_PLANE InfinitePlane;

    ASSERT(Context != NULL);

    InfinitePlane = (PCINFINITE_PLANE) Context;

    if (FaceHit > INFINITE_PLANE_BACK_FACE)
    {
        return NULL;
    }

    return InfinitePlane->Normals[FaceHit];
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS 
InfinitePlaneTraceInfinitePlane(
    _In_ PCVOID Context, 
    _In_ RAY Ray,
    _Inout_ PSHAPE_HIT_ALLOCATOR ShapeHitAllocator,
    _Outptr_result_maybenull_ PSHAPE_HIT_LIST *ShapeHitList
    )
{
    PCINFINITE_PLANE InfinitePlane;
    FLOAT DotProduct;
    FLOAT Distance;
    VECTOR3 Temp;

    ASSERT(ShapeHitAllocator != NULL);
    ASSERT(ShapeHitList != NULL);
    ASSERT(Context != NULL);

    InfinitePlane = (PCINFINITE_PLANE) Context;

    Temp = PointSubtract(Ray.Origin, InfinitePlane->Vertex);

    DotProduct = VectorDotProduct(Ray.Direction, InfinitePlane->SurfaceNormal);
    Distance = VectorDotProduct(Temp, InfinitePlane->SurfaceNormal) / -DotProduct;

    if (Distance < (FLOAT) 0.0)
    {
        *ShapeHitList = NULL;
        return ISTATUS_SUCCESS;
    }

    if (DotProduct < (FLOAT) 0.0)
    {
        *ShapeHitList = ShapeHitAllocatorAllocate(ShapeHitAllocator,
                                                  NULL,
                                                  Distance,
                                                  INFINITE_PLANE_FRONT_FACE,
                                                  NULL,
                                                  0);
    }
    else
    {
        *ShapeHitList = ShapeHitAllocatorAllocate(ShapeHitAllocator,
                                                  NULL,
                                                  Distance,
                                                  INFINITE_PLANE_BACK_FACE,
                                                  NULL,
                                                  0);
    }

    return (*ShapeHitList == NULL) ? ISTATUS_ALLOCATION_FAILED : ISTATUS_SUCCESS;
}

//
// Static variables
//

CONST STATIC DRAWING_SHAPE_VTABLE InfinitePlaneHeader = {
    { InfinitePlaneTraceInfinitePlane, NULL },
    InfinitePlaneGetTexture,
    InfinitePlaneGetNormal
};

//
// Public Functions
//

_Check_return_
_Ret_maybenull_
PDRAWING_SHAPE
InfinitePlaneAllocate(
    _In_ PCPOINT3 Vertex,
    _In_ PCVECTOR3 SurfaceNormal,
    _In_opt_ PCTEXTURE FrontTexture,
    _In_opt_ PCNORMAL FrontNormal,
    _In_opt_ PCTEXTURE BackTexture,
    _In_opt_ PCNORMAL BackNormal
    )
{
    INFINITE_PLANE InfinitePlane;
    PDRAWING_SHAPE DrawingShape;

    if (Vertex == NULL ||
        SurfaceNormal == NULL)
    {
        return NULL;
    }

    if (FrontTexture == NULL &&
        BackTexture == NULL)
    {
        return NULL;
    }

    if (VectorLength(*SurfaceNormal) == (FLOAT) 0.0)
    {
        return NULL;
    }

    InfinitePlane.Vertex = *Vertex;
    InfinitePlane.SurfaceNormal = *SurfaceNormal;
    InfinitePlane.Textures[0] = FrontTexture;
    InfinitePlane.Textures[1] = BackTexture;
    InfinitePlane.Normals[0] = FrontNormal;
    InfinitePlane.Normals[1] = BackNormal;

    DrawingShape = DrawingShapeAllocate(&InfinitePlaneHeader,
                                        &InfinitePlane,
                                        sizeof(INFINITE_PLANE),
                                        sizeof(PVOID));

    return DrawingShape;
}