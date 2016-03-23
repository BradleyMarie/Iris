/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    sphere.c

Abstract:

    This file contains the definitions for sphere types.

--*/

#include <irisphysxtoolkitp.h>
#include <iriscommon_advancedsphere.h>

//
// Types
//

typedef struct _PHYSX_SPHERE {
    PMATERIAL Materials[2];
    SPHERE Data;
} PHYSX_SPHERE, *PPHYSX_SPHERE;

typedef CONST PHYSX_SPHERE *PCPHYSX_SPHERE;

typedef struct _PHYSX_LIGHT_SPHERE {
    PMATERIAL Materials[2];
    PLIGHT Lights[2];
    SPHERE Data;
} PHYSX_LIGHT_SPHERE, *PPHYSX_LIGHT_SPHERE;

typedef CONST PHYSX_LIGHT_SPHERE *PCPHYSX_LIGHT_SPHERE;

//
// Static Functions
//

_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS 
PhysxSphereGetMaterial(
    _In_ PCVOID Context, 
    _In_ UINT32 FaceHit,
    _Out_opt_ PCMATERIAL *Material
    )
{
    PCPHYSX_SPHERE Sphere;

    ASSERT(Context != NULL);
    ASSERT(Material != NULL);

    Sphere = (PCPHYSX_SPHERE) Context;

    if (FaceHit > SPHERE_BACK_FACE)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }
    
    *Material = Sphere->Materials[FaceHit];

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS
PhysxSphereComputeNormal(
    _In_ PCVOID Context, 
    _In_ POINT3 ModelHitPoint,
    _In_ UINT32 FaceHit,
    _Out_ PVECTOR3 SurfaceNormal
    )
{
    PCPHYSX_SPHERE Sphere;
    ISTATUS Status;

    ASSERT(Context != NULL);
    ASSERT(SurfaceNormal != NULL);

    Sphere = (PCPHYSX_SPHERE) Context;
    
    Status = SphereComputeNormal(&Sphere->Data,
                                 ModelHitPoint,
                                 FaceHit,
                                 SurfaceNormal);
                                   
    return Status;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS 
PhysxSphereGetBounds(
    _In_ PCVOID Context,
    _In_ PCMATRIX ModelToWorld,
    _In_ BOUNDING_BOX WorldAlignedBoundingBox,
    _Out_ PBOOL IsInsideBox
    )
{
    PCPHYSX_SPHERE Sphere;
    ISTATUS Status;

    ASSERT(Context != NULL);
    ASSERT(IsInsideBox != NULL);

    Sphere = (PCPHYSX_SPHERE) Context;
    
    Status = SphereCheckBounds(&Sphere->Data,
                               ModelToWorld,
                               WorldAlignedBoundingBox,
                               IsInsideBox);
                                 
    return Status;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS 
PhysxSphereTestRay(
    _In_opt_ PCVOID Context, 
    _In_ RAY Ray,
    _Inout_ PHIT_ALLOCATOR HitAllocator,
    _Outptr_result_maybenull_ PHIT_LIST *HitList
    )
{
    PCPHYSX_SPHERE Sphere;
    ISTATUS Status;

    ASSERT(Context != NULL);
    ASSERT(RayValidate(Ray) != FALSE);
    ASSERT(HitAllocator != NULL);
    ASSERT(HitList != NULL);

    Sphere = (PCPHYSX_SPHERE) Context;

    Status = SphereTestRay(&Sphere->Data,
                           Ray,
                           HitAllocator,
                           HitList);

    return Status;
}

STATIC
VOID
PhysxSphereFree(
    _In_ _Post_invalid_ PVOID Context
    )
{
    PCPHYSX_SPHERE Sphere;

    ASSERT(Context != NULL);

    Sphere = (PCPHYSX_SPHERE) Context;

    MaterialRelease(Sphere->Materials[SPHERE_FRONT_FACE]);
    MaterialRelease(Sphere->Materials[SPHERE_BACK_FACE]);
}

_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS 
PhysxLightSphereGetMaterial(
    _In_ PCVOID Context, 
    _In_ UINT32 FaceHit,
    _Out_opt_ PCMATERIAL *Material
    )
{
    PCPHYSX_LIGHT_SPHERE Sphere;

    ASSERT(Context != NULL);

    Sphere = (PCPHYSX_LIGHT_SPHERE) Context;
    
    if (FaceHit > SPHERE_BACK_FACE)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }
    
    *Material = Sphere->Materials[FaceHit];

    return ISTATUS_SUCCESS;
}

_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS
PhysxLightSphereGetLight(
    _In_opt_ PCVOID Context, 
    _In_ UINT32 FaceHit,
    _Out_ PCLIGHT *Light
    )
{
    PCPHYSX_LIGHT_SPHERE Sphere;

    ASSERT(Context != NULL);
    ASSERT(Light != NULL);

    Sphere = (PCPHYSX_LIGHT_SPHERE) Context;
    
    if (FaceHit > SPHERE_BACK_FACE)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }
    
    *Light = Sphere->Lights[FaceHit];

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS
PhysxLightSphereComputeNormal(
    _In_ PCVOID Context, 
    _In_ POINT3 ModelHitPoint,
    _In_ UINT32 FaceHit,
    _Out_ PVECTOR3 SurfaceNormal
    )
{
    PCPHYSX_LIGHT_SPHERE Sphere;
    ISTATUS Status;

    ASSERT(Context != NULL);
    ASSERT(SurfaceNormal != NULL);

    Sphere = (PCPHYSX_LIGHT_SPHERE) Context;
    
    Status = SphereComputeNormal(&Sphere->Data,
                                 ModelHitPoint,
                                 FaceHit,
                                 SurfaceNormal);
                                   
    return Status;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS 
PhysxLightSphereGetBounds(
    _In_ PCVOID Context,
    _In_ PCMATRIX ModelToWorld,
    _In_ BOUNDING_BOX WorldAlignedBoundingBox,
    _Out_ PBOOL IsInsideBox
    )
{
    PCPHYSX_LIGHT_SPHERE Sphere;
    ISTATUS Status;

    ASSERT(Context != NULL);
    ASSERT(IsInsideBox != NULL);

    Sphere = (PCPHYSX_LIGHT_SPHERE) Context;
    
    Status = SphereCheckBounds(&Sphere->Data,
                               ModelToWorld,
                               WorldAlignedBoundingBox,
                               IsInsideBox);
                                 
    return Status;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS 
PhysxLightSphereTestRay(
    _In_opt_ PCVOID Context, 
    _In_ RAY Ray,
    _Inout_ PHIT_ALLOCATOR HitAllocator,
    _Outptr_result_maybenull_ PHIT_LIST *HitList
    )
{
    PCPHYSX_LIGHT_SPHERE Sphere;
    ISTATUS Status;

    ASSERT(Context != NULL);
    ASSERT(RayValidate(Ray) != FALSE);
    ASSERT(HitAllocator != NULL);
    ASSERT(HitList != NULL);

    Sphere = (PCPHYSX_LIGHT_SPHERE) Context;

    Status = SphereTestRay(&Sphere->Data,
                           Ray,
                           HitAllocator,
                           HitList);

    return Status;
}

STATIC
VOID
PhysxLightSphereFree(
    _In_ _Post_invalid_ PVOID Context
    )
{
    PCPHYSX_LIGHT_SPHERE Sphere;

    ASSERT(Context != NULL);

    Sphere = (PCPHYSX_LIGHT_SPHERE) Context;

    MaterialRelease(Sphere->Materials[SPHERE_FRONT_FACE]);
    MaterialRelease(Sphere->Materials[SPHERE_BACK_FACE]);
    LightRelease(Sphere->Lights[SPHERE_FRONT_FACE]);
    LightRelease(Sphere->Lights[SPHERE_BACK_FACE]);
}

//
// Static Variables
//

CONST STATIC PBR_SHAPE_VTABLE SphereHeader = {
    { { PhysxSphereTestRay,
        PhysxSphereFree },
      PhysxSphereComputeNormal,
      PhysxSphereGetBounds },
    PhysxSphereGetMaterial,
    NULL
};

CONST STATIC PBR_SHAPE_VTABLE LightSphereHeader = {
    { { PhysxLightSphereTestRay,
        PhysxLightSphereFree },
      PhysxLightSphereComputeNormal,
      PhysxLightSphereGetBounds },
    PhysxLightSphereGetMaterial,
    PhysxLightSphereGetLight
};

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PhysxSphereAllocate(
    _In_ POINT3 Center,
    _In_ FLOAT Radius,
    _In_opt_ PMATERIAL FrontMaterial,
    _In_opt_ PMATERIAL BackMaterial,
    _In_opt_ PLIGHT FrontLight,
    _In_opt_ PLIGHT BackLight,
    _Out_ PPBR_SHAPE *Shape
    )
{
    PCPBR_SHAPE_VTABLE ShapeVTable;
    PHYSX_LIGHT_SPHERE LightSphere;
    PHYSX_SPHERE Sphere;
    SIZE_T DataAlignment;
    FLOAT RadiusSquared;
    SIZE_T DataSize;
    ISTATUS Status;
    PCVOID Data;

    if (PointValidate(Center) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }
    
    if (IsFiniteFloat(Radius) == FALSE ||
        IsGreaterThanZeroFloat(Radius) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }
    
    RadiusSquared = Radius * Radius;
    
    if (IsFiniteFloat(Radius) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }
    
    if (Shape == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_06;
    }

    if (FrontLight != NULL || 
        BackLight != NULL)
    {
        SphereInitialize(&LightSphere.Data,
                         Center,
                         Radius);
        
        LightSphere.Materials[SPHERE_FRONT_FACE] = FrontMaterial;
        LightSphere.Materials[SPHERE_BACK_FACE] = BackMaterial;
        LightSphere.Lights[SPHERE_FRONT_FACE] = FrontLight;
        LightSphere.Lights[SPHERE_BACK_FACE] = BackLight;
        
        Data = &LightSphere;
        DataSize = sizeof(PHYSX_LIGHT_SPHERE);
        DataAlignment = _Alignof(PHYSX_LIGHT_SPHERE);
        ShapeVTable = &LightSphereHeader;
    }
    else
    {
        SphereInitialize(&Sphere.Data,
                         Center,
                         Radius);
        
        Sphere.Materials[SPHERE_FRONT_FACE] = FrontMaterial;
        Sphere.Materials[SPHERE_BACK_FACE] = BackMaterial;
        
        Data = &Sphere;
        DataSize = sizeof(PHYSX_SPHERE);
        DataAlignment = _Alignof(PHYSX_SPHERE);
        ShapeVTable = &SphereHeader;
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