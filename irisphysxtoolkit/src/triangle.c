/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    triangle.c

Abstract:

    This file contains the definitions for triangle types.

--*/

#include <irisphysxtoolkitp.h>
#include <iriscommon_advancedtriangle.h>

//
// Types
//

typedef struct _PHYSX_TRIANGLE {
    PMATERIAL Materials[2];
    TRIANGLE Data;
} PHYSX_TRIANGLE, *PPHYSX_TRIANGLE;

typedef CONST PHYSX_TRIANGLE *PCPHYSX_TRIANGLE;

typedef struct _PHYSX_LIGHT_TRIANGLE {
    PMATERIAL Materials[2];
    PLIGHT Lights[2];
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
    _Out_opt_ PCMATERIAL *Material
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
PhysxTriangleGetBounds(
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
    _Inout_ PHIT_ALLOCATOR HitAllocator,
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
    _Inout_ PHIT_ALLOCATOR HitAllocator,
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
    _Inout_ PHIT_ALLOCATOR HitAllocator,
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

    MaterialRelease(Triangle->Materials[TRIANGLE_FRONT_FACE]);
    MaterialRelease(Triangle->Materials[TRIANGLE_BACK_FACE]);
}

_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS 
PhysxLightTriangleGetMaterial(
    _In_ PCVOID Context, 
    _In_ UINT32 FaceHit,
    _Out_opt_ PCMATERIAL *Material
    )
{
    PCPHYSX_LIGHT_TRIANGLE Triangle;

    ASSERT(Context != NULL);

    Triangle = (PCPHYSX_LIGHT_TRIANGLE) Context;
    
    if (FaceHit > TRIANGLE_BACK_FACE)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }
    
    *Material = Triangle->Materials[FaceHit];

    return ISTATUS_SUCCESS;
}

_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS
PhysxLightTriangleGetLight(
    _In_opt_ PCVOID Context, 
    _In_ UINT32 FaceHit,
    _Out_ PCLIGHT *Light
    )
{
    PCPHYSX_LIGHT_TRIANGLE Triangle;

    ASSERT(Context != NULL);
    ASSERT(Light != NULL);

    Triangle = (PCPHYSX_LIGHT_TRIANGLE) Context;
    
    if (FaceHit > TRIANGLE_BACK_FACE)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }
    
    *Light = Triangle->Lights[FaceHit];

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS
PhysxLightTriangleComputeNormal(
    _In_ PCVOID Context, 
    _In_ POINT3 ModelHitPoint,
    _In_ UINT32 FaceHit,
    _Out_ PVECTOR3 SurfaceNormal
    )
{
    PCPHYSX_LIGHT_TRIANGLE Triangle;
    ISTATUS Status;

    ASSERT(Context != NULL);
    ASSERT(SurfaceNormal != NULL);

    Triangle = (PCPHYSX_LIGHT_TRIANGLE) Context;
    
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
PhysxLightTriangleGetBounds(
    _In_ PCVOID Context,
    _In_ PCMATRIX ModelToWorld,
    _In_ BOUNDING_BOX WorldAlignedBoundingBox,
    _Out_ PBOOL IsInsideBox
    )
{
    PCPHYSX_LIGHT_TRIANGLE Triangle;
    ISTATUS Status;

    ASSERT(Context != NULL);
    ASSERT(IsInsideBox != NULL);

    Triangle = (PCPHYSX_LIGHT_TRIANGLE) Context;
    
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
PhysxLightTriangleXDominantTestRay(
    _In_opt_ PCVOID Context, 
    _In_ RAY Ray,
    _Inout_ PHIT_ALLOCATOR HitAllocator,
    _Outptr_result_maybenull_ PHIT_LIST *HitList
    )
{
    PCPHYSX_LIGHT_TRIANGLE Triangle;
    ISTATUS Status;

    ASSERT(Context != NULL);
    ASSERT(RayValidate(Ray) != FALSE);
    ASSERT(HitAllocator != NULL);
    ASSERT(HitList != NULL);

    Triangle = (PCPHYSX_LIGHT_TRIANGLE) Context;

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
PhysxLightTriangleYDominantTestRay(
    _In_opt_ PCVOID Context, 
    _In_ RAY Ray,
    _Inout_ PHIT_ALLOCATOR HitAllocator,
    _Outptr_result_maybenull_ PHIT_LIST *HitList
    )
{
    PCPHYSX_LIGHT_TRIANGLE Triangle;
    ISTATUS Status;

    ASSERT(Context != NULL);
    ASSERT(RayValidate(Ray) != FALSE);
    ASSERT(HitAllocator != NULL);
    ASSERT(HitList != NULL);

    Triangle = (PCPHYSX_LIGHT_TRIANGLE) Context;

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
PhysxLightTriangleZDominantTestRay(
    _In_opt_ PCVOID Context, 
    _In_ RAY Ray,
    _Inout_ PHIT_ALLOCATOR HitAllocator,
    _Outptr_result_maybenull_ PHIT_LIST *HitList
    )
{
    PCPHYSX_LIGHT_TRIANGLE Triangle;
    ISTATUS Status;

    ASSERT(Context != NULL);
    ASSERT(RayValidate(Ray) != FALSE);
    ASSERT(HitAllocator != NULL);
    ASSERT(HitList != NULL);

    Triangle = (PCPHYSX_LIGHT_TRIANGLE) Context;

    Status = TriangleZDominantTestRay(&Triangle->Data,
                                      Ray,
                                      HitAllocator,
                                      HitList);

    return Status;
}

STATIC
VOID
PhysxLightTriangleFree(
    _In_ _Post_invalid_ PVOID Context
    )
{
    PCPHYSX_LIGHT_TRIANGLE Triangle;

    ASSERT(Context != NULL);

    Triangle = (PCPHYSX_LIGHT_TRIANGLE) Context;

    MaterialRelease(Triangle->Materials[TRIANGLE_FRONT_FACE]);
    MaterialRelease(Triangle->Materials[TRIANGLE_BACK_FACE]);
    LightRelease(Triangle->Lights[TRIANGLE_FRONT_FACE]);
    LightRelease(Triangle->Lights[TRIANGLE_BACK_FACE]);
}

//
// Static Variables
//

CONST STATIC PBR_GEOMETRY_VTABLE XTriangleHeader = {
    PhysxTriangleXDominantTestRay,
    PhysxTriangleFree,
    PhysxTriangleComputeNormal,
    PhysxTriangleGetBounds,
    PhysxTriangleGetMaterial,
    NULL
};

CONST STATIC PBR_GEOMETRY_VTABLE YTriangleHeader = {
    PhysxTriangleYDominantTestRay,
    PhysxTriangleFree,
    PhysxTriangleComputeNormal,
    PhysxTriangleGetBounds,
    PhysxTriangleGetMaterial,
    NULL
};

CONST STATIC PBR_GEOMETRY_VTABLE ZTriangleHeader = {
    PhysxTriangleZDominantTestRay,
    PhysxTriangleFree,
    PhysxTriangleComputeNormal,
    PhysxTriangleGetBounds,
    PhysxTriangleGetMaterial,
    NULL
};

CONST STATIC PBR_GEOMETRY_VTABLE XLightTriangleHeader = {
    PhysxLightTriangleXDominantTestRay,
    PhysxLightTriangleFree,
    PhysxLightTriangleComputeNormal,
    PhysxLightTriangleGetBounds,
    PhysxLightTriangleGetMaterial,
    PhysxLightTriangleGetLight
};

CONST STATIC PBR_GEOMETRY_VTABLE YLightTriangleHeader = {
    PhysxLightTriangleYDominantTestRay,
    PhysxLightTriangleFree,
    PhysxLightTriangleComputeNormal,
    PhysxLightTriangleGetBounds,
    PhysxLightTriangleGetMaterial,
    PhysxLightTriangleGetLight
};

CONST STATIC PBR_GEOMETRY_VTABLE ZLightTriangleHeader = {
    PhysxLightTriangleXDominantTestRay,
    PhysxLightTriangleFree,
    PhysxLightTriangleComputeNormal,
    PhysxLightTriangleGetBounds,
    PhysxLightTriangleGetMaterial,
    PhysxLightTriangleGetLight
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
    _In_opt_ PMATERIAL FrontMaterial,
    _In_opt_ PMATERIAL BackMaterial,
    _In_opt_ PLIGHT FrontLight,
    _In_opt_ PLIGHT BackLight,
    _Out_ PPBR_GEOMETRY *Shape
    )
{
    PCPBR_GEOMETRY_VTABLE ShapeVTable;
    PHYSX_LIGHT_TRIANGLE LightTriangle;
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
    
    if (Shape == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_07;
    }

    if (FrontLight != NULL || 
        BackLight != NULL)
    {
        Status = TriangleInitialize(&LightTriangle.Data,
                                    Vertex0,
                                    Vertex1,
                                    Vertex2,
                                    &DominantAxis);

        if (Status != ISTATUS_SUCCESS)
        {
            return Status;
        }
        
        LightTriangle.Materials[TRIANGLE_FRONT_FACE] = FrontMaterial;
        LightTriangle.Materials[TRIANGLE_BACK_FACE] = BackMaterial;
        LightTriangle.Lights[TRIANGLE_FRONT_FACE] = FrontLight;
        LightTriangle.Lights[TRIANGLE_BACK_FACE] = BackLight;
        
        Data = &LightTriangle;
        DataSize = sizeof(PHYSX_LIGHT_TRIANGLE);
        DataAlignment = _Alignof(PHYSX_LIGHT_TRIANGLE);
        
        switch (DominantAxis)
        {
            case VECTOR_X_AXIS:
                ShapeVTable = &XLightTriangleHeader;
                break;
            case VECTOR_Y_AXIS:
                ShapeVTable = &YLightTriangleHeader;
                break;
            default:
                ShapeVTable = &ZLightTriangleHeader;
                break;
        }
    }
    else
    {
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
                ShapeVTable = &XTriangleHeader;
                break;
            case VECTOR_Y_AXIS:
                ShapeVTable = &YTriangleHeader;
                break;
            default:
                ShapeVTable = &ZTriangleHeader;
                break;
        }
    }
    
    Status = PBRShapeAllocate(ShapeVTable,
                              Data,
                              DataSize,
                              DataAlignment,
                              Shape);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    MaterialRetain(FrontMaterial);
    MaterialRetain(BackMaterial);
    LightRetain(FrontLight);
    LightRetain(BackLight);

    return ISTATUS_SUCCESS;
}