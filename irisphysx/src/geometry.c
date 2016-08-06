/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    geometry.c

Abstract:

    This file contains the definitions for the PBR_GEOMETRY type.

--*/

#include <irisphysxp.h>

//
// Types
//

typedef union _PHYSX_GEOMETRY_SHARED_REFERENCE_COUNT {
    PPHYSX_LIGHTED_GEOMETRY Owner;
    SIZE_T Value;
} PHYSX_GEOMETRY_SHARED_REFERENCE_COUNT;

struct _PBR_GEOMETRY {
    PCPBR_GEOMETRY_VTABLE VTable;
    PHYSX_GEOMETRY_SHARED_REFERENCE_COUNT ReferenceCount;
    PVOID Data;
};

typedef struct _PHYSX_GEOMETRY_WITH_LIGHTS {
    PPBR_GEOMETRY Geometry;
    _Field_size_(NumberOfLights) PPBR_LIGHT Lights;
    SIZE_T NumberOfLights;
} PHYSX_GEOMETRY_WITH_LIGHTS, *PPHYSX_GEOMETRY_WITH_LIGHTS;

typedef CONST PHYSX_GEOMETRY_WITH_LIGHTS *PCPHYSX_GEOMETRY_WITH_LIGHTS;

//
// Static Functions
//

_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS 
PhysxGeometryWithLightsGetMaterial(
    _In_ PCVOID Context, 
    _In_ UINT32 FaceHit,
    _Out_opt_ PCPBR_MATERIAL *Material
    )
{
    PCPHYSX_GEOMETRY_WITH_LIGHTS GeometryWithLights;
    ISTATUS Status;

    ASSERT(Context != NULL);

    GeometryWithLights = (PCPHYSX_GEOMETRY_WITH_LIGHTS) Context;
    
    Status = PBRGeometryGetMaterial(GeometryWithLights->Geometry,
                                    FaceHit,
                                    Material);

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS
PhysxGeometryWithLightsComputeNormal(
    _In_ PCVOID Context, 
    _In_ POINT3 ModelHitPoint,
    _In_ UINT32 FaceHit,
    _Out_ PVECTOR3 SurfaceNormal
    )
{
    PCPHYSX_GEOMETRY_WITH_LIGHTS GeometryWithLights;
    ISTATUS Status;

    ASSERT(Context != NULL);
    ASSERT(SurfaceNormal != NULL);

    GeometryWithLights = (PCPHYSX_GEOMETRY_WITH_LIGHTS) Context;
    
    Status = PBRGeometryComputeNormal(GeometryWithLights->Geometry,
                                      ModelHitPoint,
                                      FaceHit,
                                      SurfaceNormal);
                                   
    return Status;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS 
PhysxGeometryWithLightsTestBounds(
    _In_ PCVOID Context,
    _In_ PCMATRIX ModelToWorld,
    _In_ BOUNDING_BOX WorldAlignedBoundingBox,
    _Out_ PBOOL IsInsideBox
    )
{
    PCPHYSX_GEOMETRY_WITH_LIGHTS GeometryWithLights;
    ISTATUS Status;

    ASSERT(Context != NULL);
    ASSERT(IsInsideBox != NULL);

    GeometryWithLights = (PCPHYSX_GEOMETRY_WITH_LIGHTS) Context;
    
    Status = PBRGeometryCheckBounds(GeometryWithLights->Geometry,
                                    ModelToWorld,
                                    WorldAlignedBoundingBox,
                                    IsInsideBox);
                                 
    return Status;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS 
PhysxGeometryWithLightsTestRay(
    _In_opt_ PCVOID Context, 
    _In_ RAY Ray,
    _Inout_ PPBR_HIT_ALLOCATOR HitAllocator,
    _Outptr_result_maybenull_ PHIT_LIST *HitList
    )
{
    PCPHYSX_GEOMETRY_WITH_LIGHTS GeometryWithLights;
    ISTATUS Status;

    ASSERT(Context != NULL);
    ASSERT(RayValidate(Ray) != FALSE);
    ASSERT(HitAllocator != NULL);
    ASSERT(HitList != NULL);

    GeometryWithLights = (PCPHYSX_GEOMETRY_WITH_LIGHTS) Context;

    //
    // Call directly through VTable to avoid modifying
    // the Hit Allocator to point to the nested geometry
    //

    Status = GeometryWithLights->Geometry->VTable->TestRayRoutine(GeometryWithLights->Geometry->Data,
                                                                  Ray,
                                                                  HitAllocator,
                                                                  HitList);

    return Status;
}

STATIC
VOID
PhysxGeometryWithLightsFree(
    _In_ _Post_invalid_ PVOID Context
    )
{
    PCPHYSX_GEOMETRY_WITH_LIGHTS GeometryWithLights;

    ASSERT(Context != NULL);

    GeometryWithLights = (PCPHYSX_GEOMETRY_WITH_LIGHTS) Context;

    PBRGeometryRelease(GeometryWithLights->Geometry);
}

//
// Static Variables
//

CONST STATIC PBR_GEOMETRY_VTABLE GeometryWithLightsVTable = {
    PhysxGeometryWithLightsTestRay,
    PhysxGeometryWithLightsComputeNormal,
    PhysxGeometryWithLightsTestBounds,
    PhysxGeometryWithLightsGetMaterial,
    PhysxGeometryWithLightsFree
};

//
// Internal Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PBRGeometryTestRayAdapter(
    _In_ PCVOID Context,
    _In_ RAY Ray,
    _In_ PHIT_ALLOCATOR HitAllocator,
    _Out_ PHIT_LIST *HitList
    )
{
    PCPBR_GEOMETRY PBRGeometry;
    PBR_HIT_ALLOCATOR PBRHitAllocator;
    ISTATUS Status;
    
    ASSERT(Context != NULL);
    ASSERT(RayValidate(Ray) != FALSE);
    ASSERT(HitAllocator != NULL);
    ASSERT(HitList != NULL);
    
    PBRGeometry = (PCPBR_GEOMETRY) Context;
    
    PBRHitAllocatorInitialize(&PBRHitAllocator,
                              HitAllocator,
                              PBRGeometry,
                              Ray);
    
    Status = PBRGeometry->VTable->TestRayRoutine(PBRGeometry->Data,
                                                 Ray,
                                                 &PBRHitAllocator,
                                                 HitList);

    return Status;
}

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PBRGeometryAllocate(
    _In_ PCPBR_GEOMETRY_VTABLE PBRGeometryVTable,
    _When_(DataSizeInBytes != 0, _In_reads_bytes_opt_(DataSizeInBytes)) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _When_(DataSizeInBytes != 0, _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && DataSizeInBytes % _Curr_ == 0)) SIZE_T DataAlignment,
    _Out_ PPBR_GEOMETRY *PBRGeometry
    )
{
    BOOL AllocationSuccessful;
    PVOID HeaderAllocation;
    PVOID DataAllocation;
    PPBR_GEOMETRY AllocatedGeometry;

    ASSERT(PBRGeometryVTable != &GeometryWithLightsVTable);

    if (PBRGeometryVTable == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (DataSizeInBytes != 0)
    {
        if (Data == NULL)
        {
            return ISTATUS_INVALID_ARGUMENT_COMBINATION_00;
        }

        if (DataAlignment == 0 ||
            DataAlignment & DataAlignment - 1)
        {
            return ISTATUS_INVALID_ARGUMENT_COMBINATION_01;
        }

        if (DataSizeInBytes % DataAlignment != 0)
        {
            return ISTATUS_INVALID_ARGUMENT_COMBINATION_02;
        }
    }

    if (PBRGeometry == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    AllocationSuccessful = IrisAlignedAllocWithHeader(sizeof(PBR_GEOMETRY),
                                                      _Alignof(PBR_GEOMETRY),
                                                      &HeaderAllocation,
                                                      DataSizeInBytes,
                                                      DataAlignment,
                                                      &DataAllocation,
                                                      NULL);

    if (AllocationSuccessful == FALSE)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    AllocatedGeometry = (PPBR_GEOMETRY) HeaderAllocation;

    AllocatedGeometry->VTable = PBRGeometryVTable;
    AllocatedGeometry->Data = DataAllocation;
    AllocatedGeometry->ReferenceCount.Value = 1;

    if (DataSizeInBytes != 0)
    {
        memcpy(DataAllocation, Data, DataSizeInBytes);
    }

    *PBRGeometry = AllocatedGeometry;

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
PBRGeometryTestNestedGeometry(
    _In_ PCPBR_GEOMETRY PBRGeometry,
    _In_ PPBR_HIT_ALLOCATOR PBRHitAllocator,
    _Out_ PHIT_LIST *HitList
    )
{
    PCPBR_GEOMETRY OldGeometry;
    ISTATUS Status;
    RAY Ray;
    
    if (PBRGeometry == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }
    
    if (PBRHitAllocator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (HitList == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }
    
    Ray = PBRHitAllocatorGetRay(PBRHitAllocator);
    
    OldGeometry = PBRHitAllocatorGetGeometry(PBRHitAllocator);
    PBRHitAllocatorSetGeometry(PBRHitAllocator, PBRGeometry);
    
    Status = PBRGeometry->VTable->TestRayRoutine(PBRGeometry->Data,
                                                 Ray,
                                                 PBRHitAllocator,
                                                 HitList);

    PBRHitAllocatorSetGeometry(PBRHitAllocator, OldGeometry);

    return Status;
}
    
_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS 
PBRGeometryCheckBounds(
    _In_ PCPBR_GEOMETRY PBRGeometry,
    _In_ PCMATRIX ModelToWorld,
    _In_ BOUNDING_BOX WorldAlignedBoundingBox,
    _Out_ PBOOL IsInsideBox
    )
{
    ISTATUS Status;
    
    if (PBRGeometry == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }
    
    if (BoundingBoxValidate(WorldAlignedBoundingBox) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (IsInsideBox == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }
    
    Status = PBRGeometry->VTable->CheckBoundsRoutine(PBRGeometry->Data,
                                                     ModelToWorld,
                                                     WorldAlignedBoundingBox,
                                                     IsInsideBox);

    return Status;
}
    
_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PBRGeometryComputeNormal(
    _In_ PCPBR_GEOMETRY PBRGeometry,
    _In_ POINT3 ModelHitPoint,
    _In_ UINT32 FaceHit,
    _Out_ PVECTOR3 ModelSurfaceNormal
    )
{
    ISTATUS Status;
    
    if (PBRGeometry == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }
    
    if (PointValidate(ModelHitPoint) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (ModelSurfaceNormal == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }
    
    Status = PBRGeometry->VTable->ComputeNormalRoutine(PBRGeometry->Data,
                                                       ModelHitPoint,
                                                       FaceHit,
                                                       ModelSurfaceNormal);

    return Status;
}
    
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PBRGeometryGetMaterial(
    _In_ PCPBR_GEOMETRY PBRGeometry,
    _In_ UINT32 FaceHit,
    _Outptr_result_maybenull_ PCPBR_MATERIAL *Material
    )
{
    ISTATUS Status;
    
    if (PBRGeometry == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (Material == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }
    
    Status = PBRGeometry->VTable->GetMaterialRoutine(PBRGeometry->Data,
                                                     FaceHit,
                                                     Material);

    return Status;
}
    
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PBRGeometryGetLight(
    _In_ PCPBR_GEOMETRY PBRGeometry,
    _In_ UINT32 FaceHit,
    _Outptr_result_maybenull_ PCPBR_LIGHT *Light
    )
{
    if (PBRGeometry == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (Light == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }
    
    if (PBRGeometry->VTable != &GeometryWithLightsVTable)
    {
        *Light = NULL;
        return ISTATUS_SUCCESS;
    }

    // TODO

    *Light = NULL;
    return ISTATUS_SUCCESS;
}
    
VOID
PBRGeometryRetain(
    _In_opt_ PPBR_GEOMETRY PBRGeometry
    )
{
    if (PBRGeometry == NULL)
    {
        return;
    }

    if (PBRGeometry->VTable == &GeometryWithLightsVTable)
    {
        LightedGeometryRetain(PBRGeometry->ReferenceCount.Owner);
    }
    else
    {
        PBRGeometry->ReferenceCount.Value += 1;
    }
}
    
VOID
PBRGeometryRelease(
    _In_opt_ _Post_invalid_ PPBR_GEOMETRY PBRGeometry
    )
{
    SIZE_T ReferenceCount;

    if (PBRGeometry == NULL)
    {
        return;
    }

    if (PBRGeometry->VTable == &GeometryWithLightsVTable)
    {
        LightedGeometryRelease(PBRGeometry->ReferenceCount.Owner);
    }
    else
    {
        ReferenceCount = (PBRGeometry->ReferenceCount.Value -= 1);

        if (ReferenceCount == 0)
        {
            PBRGeometryFree(PBRGeometry);
        }
    }
}

VOID
PBRGeometryFree(
    _In_ _Post_invalid_ PPBR_GEOMETRY PBRGeometry
    )
{
    PFREE_ROUTINE FreeRoutine;

    ASSERT(PBRGeometry != NULL);
    
    FreeRoutine = PBRGeometry->VTable->FreeRoutine;

    if (FreeRoutine != NULL)
    {
        FreeRoutine(PBRGeometry->Data);
    }

    IrisAlignedFree(PBRGeometry);
}