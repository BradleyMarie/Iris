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
    PTEXTURE Textures[2];
    PNORMAL Normals[2];
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
InfinitePlaneTestRay(
    _In_opt_ PCVOID Context, 
    _In_ RAY Ray,
    _Inout_ PHIT_ALLOCATOR HitAllocator,
    _Outptr_result_maybenull_ PHIT_LIST *HitList
    )
{
    FLOAT Distance;
    FLOAT DotProduct;
    PCINFINITE_PLANE InfinitePlane;
    ISTATUS Status;
    VECTOR3 Temp;

    ASSERT(HitAllocator != NULL);
    ASSERT(HitList != NULL);
    ASSERT(Context != NULL);

    InfinitePlane = (PCINFINITE_PLANE) Context;

    Temp = PointSubtract(Ray.Origin, InfinitePlane->Vertex);

    DotProduct = VectorDotProduct(Ray.Direction, InfinitePlane->SurfaceNormal);
    Distance = VectorDotProduct(Temp, InfinitePlane->SurfaceNormal) / -DotProduct;

    if (Distance < (FLOAT) 0.0)
    {
        *HitList = NULL;
        return ISTATUS_SUCCESS;
    }

    if (DotProduct < (FLOAT) 0.0)
    {
        Status = HitAllocatorAllocate(HitAllocator,
                                      NULL,
                                      Distance,
                                      INFINITE_PLANE_FRONT_FACE,
                                      NULL,
                                      0,
                                      0,
                                      HitList);
    }
    else
    {
        Status = HitAllocatorAllocate(HitAllocator,
                                      NULL,
                                      Distance,
                                      INFINITE_PLANE_BACK_FACE,
                                      NULL,
                                      0,
                                      0,
                                      HitList);
    }

    return Status;
}

STATIC
InfinitePlaneFree(
    _In_ _Post_invalid_ PVOID Context
    )
{
    PINFINITE_PLANE InfinitePlane;

    ASSERT(Context != NULL);

    InfinitePlane = (PINFINITE_PLANE) Context;

    TextureDereference(InfinitePlane->Textures[INFINITE_PLANE_FRONT_FACE]);
    TextureDereference(InfinitePlane->Textures[INFINITE_PLANE_BACK_FACE]);
    NormalDereference(InfinitePlane->Normals[INFINITE_PLANE_FRONT_FACE]);
    NormalDereference(InfinitePlane->Normals[INFINITE_PLANE_BACK_FACE]);
}

//
// Static variables
//

CONST STATIC DRAWING_SHAPE_VTABLE InfinitePlaneHeader = {
    { InfinitePlaneTestRay, NULL },
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
    _In_ POINT3 Vertex,
    _In_ VECTOR3 SurfaceNormal,
    _In_opt_ PTEXTURE FrontTexture,
    _In_opt_ PNORMAL FrontNormal,
    _In_opt_ PTEXTURE BackTexture,
    _In_opt_ PNORMAL BackNormal
    )
{
    PDRAWING_SHAPE DrawingShape;
    INFINITE_PLANE InfinitePlane;

    if (PointValidate(Vertex) == FALSE ||
        VectorValidate(SurfaceNormal) == FALSE)
    {
        return NULL;
    }

    if (FrontTexture == NULL &&
        BackTexture == NULL)
    {
        return NULL;
    }

    SurfaceNormal = VectorNormalize(SurfaceNormal, NULL, NULL);

    InfinitePlane.Vertex = Vertex;
    InfinitePlane.SurfaceNormal = SurfaceNormal;
    InfinitePlane.Textures[INFINITE_PLANE_FRONT_FACE] = FrontTexture;
    InfinitePlane.Textures[INFINITE_PLANE_BACK_FACE] = BackTexture;
    InfinitePlane.Normals[INFINITE_PLANE_FRONT_FACE] = FrontNormal;
    InfinitePlane.Normals[INFINITE_PLANE_BACK_FACE] = BackNormal;

    DrawingShape = DrawingShapeAllocate(&InfinitePlaneHeader,
                                        &InfinitePlane,
                                        sizeof(INFINITE_PLANE),
                                        sizeof(PVOID));

    if (DrawingShape != NULL)
    {
        TextureReference(FrontTexture);
        TextureReference(BackTexture);
        NormalReference(FrontNormal);
        NormalDereference(BackNormal);
    }

    return DrawingShape;
}