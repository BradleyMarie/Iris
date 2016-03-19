/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    iristoolkit_triangle.h

Abstract:

    This file contains the definitions for the IRISTOOLKIT_TRIANGLE type.

--*/

#include <iristoolkitp.h>
#include <iriscommon_triangle.h>

//
// Constants
//

#define ISTATUS_INVALID_ARGUMENT     0x05 // Old Error

//
// Types
//

typedef struct _IRISTOOLKIT_TRIANGLE {
    PTEXTURE Textures[2];
    PNORMAL Normals[2];
    TRIANGLE Data;
} IRISTOOLKIT_TRIANGLE, *PIRISTOOLKIT_TRIANGLE;

typedef CONST IRISTOOLKIT_TRIANGLE *PCIRISTOOLKIT_TRIANGLE;

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
    PCIRISTOOLKIT_TRIANGLE Triangle;

    ASSERT(Context != NULL);

    Triangle = (PCIRISTOOLKIT_TRIANGLE) Context;

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
    PCIRISTOOLKIT_TRIANGLE Triangle;

    ASSERT(Context != NULL);

    Triangle = (PCIRISTOOLKIT_TRIANGLE) Context;

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
TriangleXTestRay(
    _In_opt_ PCVOID Context, 
    _In_ RAY Ray,
    _Inout_ PHIT_ALLOCATOR HitAllocator,
    _Outptr_result_maybenull_ PHIT_LIST *HitList
    )
{
    PCIRISTOOLKIT_TRIANGLE Triangle;
    ISTATUS Status;

    ASSERT(HitAllocator != NULL);
    ASSERT(HitList != NULL);
    ASSERT(Context != NULL);

    Triangle = (PCIRISTOOLKIT_TRIANGLE) Context;

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
TriangleYTestRay(
    _In_opt_ PCVOID Context, 
    _In_ RAY Ray,
    _Inout_ PHIT_ALLOCATOR HitAllocator,
    _Outptr_result_maybenull_ PHIT_LIST *HitList
    )
{
    PCIRISTOOLKIT_TRIANGLE Triangle;
    ISTATUS Status;

    ASSERT(HitAllocator != NULL);
    ASSERT(HitList != NULL);
    ASSERT(Context != NULL);

    Triangle = (PCIRISTOOLKIT_TRIANGLE) Context;

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
TriangleZTestRay(
    _In_opt_ PCVOID Context, 
    _In_ RAY Ray,
    _Inout_ PHIT_ALLOCATOR HitAllocator,
    _Outptr_result_maybenull_ PHIT_LIST *HitList
    )
{
    PCIRISTOOLKIT_TRIANGLE Triangle;
    ISTATUS Status;

    ASSERT(HitAllocator != NULL);
    ASSERT(HitList != NULL);
    ASSERT(Context != NULL);

    Triangle = (PCIRISTOOLKIT_TRIANGLE) Context;

    Status = TriangleZDominantTestRay(&Triangle->Data,
                                      Ray,
                                      HitAllocator,
                                      HitList);

    return Status;
}

STATIC
VOID
TriangleFree(
    _In_ _Post_invalid_ PVOID Context
    )
{
    PCIRISTOOLKIT_TRIANGLE Triangle;

    ASSERT(Context != NULL);

    Triangle = (PCIRISTOOLKIT_TRIANGLE) Context;

    TextureDereference(Triangle->Textures[TRIANGLE_FRONT_FACE]);
    TextureDereference(Triangle->Textures[TRIANGLE_BACK_FACE]);
    NormalDereference(Triangle->Normals[TRIANGLE_FRONT_FACE]);
    NormalDereference(Triangle->Normals[TRIANGLE_BACK_FACE]);
}

//
// Static variables
//

CONST STATIC DRAWING_SHAPE_VTABLE XTriangleHeader = {
    { TriangleZDominantTestRay, TriangleFree },
    TriangleGetTexture,
    TriangleGetNormal
};

CONST STATIC DRAWING_SHAPE_VTABLE YTriangleHeader = {
    { TriangleYTestRay, TriangleFree },
    TriangleGetTexture,
    TriangleGetNormal
};

CONST STATIC DRAWING_SHAPE_VTABLE ZTriangleHeader = {
    { TriangleZTestRay, TriangleFree },
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
IrisToolkitTriangleInitialize(
    _Out_ PIRISTOOLKIT_TRIANGLE Triangle,
    _In_ POINT3 Vertex0,
    _In_ POINT3 Vertex1,
    _In_ POINT3 Vertex2,
    _In_opt_ PTEXTURE FrontTexture,
    _In_opt_ PTEXTURE BackTexture,
    _Out_ PVECTOR_AXIS DominantAxis
    )
{
    ISTATUS Status;

    if (PointValidate(Vertex0) == FALSE ||
        PointValidate(Vertex1) == FALSE ||
        PointValidate(Vertex2) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT;
    }

    if (FrontTexture == NULL &&
        BackTexture == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT;
    }

    Status = TriangleInitialize(&Triangle->Data,
                                Vertex0,
                                Vertex1,
                                Vertex2,
                                DominantAxis);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
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
    _In_ POINT3 Vertex0,
    _In_ POINT3 Vertex1,
    _In_ POINT3 Vertex2,
    _In_opt_ PTEXTURE FrontTexture,
    _In_opt_ PNORMAL FrontNormal,
    _In_opt_ PTEXTURE BackTexture,
    _In_opt_ PNORMAL BackNormal
    )
{
    VECTOR_AXIS DominantAxis;
    PDRAWING_SHAPE DrawingShape;
    ISTATUS Status;
    PCDRAWING_SHAPE_VTABLE TriangleHeader;
    IRISTOOLKIT_TRIANGLE Triangle;

    //
    // Argument verification done by TriangleInitialize
    //

    Status = IrisToolkitTriangleInitialize(&Triangle,
                                           Vertex0,
                                           Vertex1,
                                           Vertex2,
                                           FrontTexture,
                                           BackTexture,
                                           &DominantAxis);

    if (Status != ISTATUS_SUCCESS)
    {
        return NULL;
    }

    Triangle.Normals[TRIANGLE_FRONT_FACE] = FrontNormal;
    Triangle.Normals[TRIANGLE_BACK_FACE] = BackNormal;

    switch (DominantAxis)
    {
        case VECTOR_X_AXIS:
            TriangleHeader = &XTriangleHeader;
            break;
        case VECTOR_Y_AXIS:
            TriangleHeader = &YTriangleHeader;
            break;
        default:
            TriangleHeader = &ZTriangleHeader;
            break;
    }
    
    DrawingShape = DrawingShapeAllocate(TriangleHeader,
                                        &Triangle,
                                        sizeof(IRISTOOLKIT_TRIANGLE),
                                        _Alignof(IRISTOOLKIT_TRIANGLE));

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
    _In_ POINT3 Vertex0,
    _In_ POINT3 Vertex1,
    _In_ POINT3 Vertex2,
    _In_opt_ PTEXTURE FrontTexture,
    _In_opt_ PTEXTURE BackTexture,
    _Out_opt_ PNORMAL *FrontNormal,
    _Out_opt_ PNORMAL *BackNormal
    )
{
    PNORMAL AllocatedBackNormal;
    PNORMAL AllocatedFrontNormal;
    VECTOR_AXIS DominantAxis;
    PDRAWING_SHAPE DrawingShape;
    VECTOR3 FrontSurfaceNormal;
    ISTATUS Status;
    IRISTOOLKIT_TRIANGLE Triangle;
    PCDRAWING_SHAPE_VTABLE TriangleHeader;

    //
    // Argument verification done by TriangleInitialize
    //

    Status = IrisToolkitTriangleInitialize(&Triangle,
                                           Vertex0,
                                           Vertex1,
                                           Vertex2,
                                           FrontTexture,
                                           BackTexture,
                                           &DominantAxis);

    if (Status != ISTATUS_SUCCESS)
    {
        return NULL;
    }

    FrontSurfaceNormal = Triangle.Data.SurfaceNormal;

    switch (DominantAxis)
    {
        case VECTOR_X_AXIS:
            TriangleHeader = &XTriangleHeader;
            break;
        case VECTOR_Y_AXIS:
            TriangleHeader = &YTriangleHeader;
            break;
        default:
            TriangleHeader = &ZTriangleHeader;
            break;
    }

    if (FrontTexture != NULL)
    {
        AllocatedFrontNormal = ConstantNormalAllocate(FrontSurfaceNormal,
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

        AllocatedBackNormal = ConstantNormalAllocate(FrontSurfaceNormal,
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

    DrawingShape = DrawingShapeAllocate(TriangleHeader,
                                        &Triangle,
                                        sizeof(IRISTOOLKIT_TRIANGLE),
                                        _Alignof(IRISTOOLKIT_TRIANGLE));

    if (DrawingShape != NULL)
    {
        TextureReference(FrontTexture);
        TextureReference(BackTexture);
        NormalReference(AllocatedFrontNormal);
        NormalReference(AllocatedBackNormal);
    }
    else
    {
        NormalDereference(AllocatedFrontNormal);
        NormalDereference(AllocatedBackNormal);
    }

    return DrawingShape;
}