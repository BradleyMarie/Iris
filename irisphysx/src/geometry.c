/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    geometry.c

Abstract:

    This file contains the definitions for the PBR_GEOMETRY type.

--*/

#include <irisphysxp.h>

//
// Macros
//

#define PHYSX_LIGHTED_GEOMETRY_MAX_ATTACHED_LIGHTS \
    (SIZE_MAX / sizeof(PHYSX_LIGHTED_GEOMETRY))

//
// Types
//

typedef struct _PHYSX_ATTACHED_LIGHT {
    PPBR_LIGHT Light;
    UINT32 Face;
} PHYSX_ATTACHED_LIGHT, *PPHYSX_ATTACHED_LIGHT;

typedef CONST PHYSX_ATTACHED_LIGHT *PCPHYSX_ATTACHED_LIGHT;

typedef struct _PHYSX_LIGHTED_GEOMETRY {
    PPBR_GEOMETRY Geometry;
    _Field_size_(NumberOfLights) PPHYSX_ATTACHED_LIGHT Lights;
    SIZE_T NumberOfLights;
} PHYSX_LIGHTED_GEOMETRY, *PPHYSX_LIGHTED_GEOMETRY;

typedef CONST PHYSX_LIGHTED_GEOMETRY *PCPHYSX_LIGHTED_GEOMETRY;

typedef union _PHYSX_GEOMETRY_SHARED_REFERENCE_COUNT {
    PPHYSX_AREA_LIGHT_REFERENCE_COUNT Area;
    SIZE_T Simple;
} PHYSX_GEOMETRY_SHARED_REFERENCE_COUNT;

struct _PBR_GEOMETRY {
    PCPBR_GEOMETRY_VTABLE VTable;
    PHYSX_GEOMETRY_SHARED_REFERENCE_COUNT ReferenceCount;
    PVOID Data;
};

//
// Lighted Geometry Static Functions
//

_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS 
PhysxLightedGeometryGetMaterial(
    _In_ PCVOID Context, 
    _In_ UINT32 FaceHit,
    _Out_opt_ PCPBR_MATERIAL *Material
    )
{
    PCPHYSX_LIGHTED_GEOMETRY LightedGeometry;
    ISTATUS Status;

    ASSERT(Context != NULL);

    LightedGeometry = (PCPHYSX_LIGHTED_GEOMETRY) Context;
    
    Status = PBRGeometryGetMaterial(LightedGeometry->Geometry,
                                    FaceHit,
                                    Material);

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS
PhysxLightedGeometryComputeNormal(
    _In_ PCVOID Context, 
    _In_ POINT3 ModelHitPoint,
    _In_ UINT32 FaceHit,
    _Out_ PVECTOR3 SurfaceNormal
    )
{
    PCPHYSX_LIGHTED_GEOMETRY LightedGeometry;
    ISTATUS Status;

    ASSERT(Context != NULL);
    ASSERT(SurfaceNormal != NULL);

    LightedGeometry = (PCPHYSX_LIGHTED_GEOMETRY) Context;
    
    Status = PBRGeometryComputeNormal(LightedGeometry->Geometry,
                                      ModelHitPoint,
                                      FaceHit,
                                      SurfaceNormal);
                                   
    return Status;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS 
PhysxLightedGeometryTestBounds(
    _In_ PCVOID Context,
    _In_ PCMATRIX ModelToWorld,
    _In_ BOUNDING_BOX WorldAlignedBoundingBox,
    _Out_ PBOOL IsInsideBox
    )
{
    PCPHYSX_LIGHTED_GEOMETRY LightedGeometry;
    ISTATUS Status;

    ASSERT(Context != NULL);
    ASSERT(IsInsideBox != NULL);

    LightedGeometry = (PCPHYSX_LIGHTED_GEOMETRY) Context;
    
    Status = PBRGeometryCheckBounds(LightedGeometry->Geometry,
                                    ModelToWorld,
                                    WorldAlignedBoundingBox,
                                    IsInsideBox);
                                 
    return Status;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS 
PhysxLightedGeometryTestRay(
    _In_opt_ PCVOID Context, 
    _In_ RAY Ray,
    _Inout_ PPBR_HIT_ALLOCATOR HitAllocator,
    _Outptr_result_maybenull_ PHIT_LIST *HitList
    )
{
    PCPHYSX_LIGHTED_GEOMETRY LightedGeometry;
    ISTATUS Status;

    ASSERT(Context != NULL);
    ASSERT(RayValidate(Ray) != FALSE);
    ASSERT(HitAllocator != NULL);
    ASSERT(HitList != NULL);

    LightedGeometry = (PCPHYSX_LIGHTED_GEOMETRY) Context;

    //
    // Call directly through VTable to avoid modifying
    // the Hit Allocator to point to the nested geometry
    //

    Status = LightedGeometry->Geometry->VTable->TestRayRoutine(LightedGeometry->Geometry->Data,
                                                               Ray,
                                                               HitAllocator,
                                                               HitList);

    return Status;
}

STATIC
VOID
PhysxLightedGeometryFree(
    _In_ _Post_invalid_ PVOID Context
    )
{
    PCPHYSX_LIGHTED_GEOMETRY LightedGeometry;

    ASSERT(Context != NULL);

    LightedGeometry = (PCPHYSX_LIGHTED_GEOMETRY) Context;

    PBRGeometryRelease(LightedGeometry->Geometry);
}

//
// Static Variables
//

CONST STATIC PBR_GEOMETRY_VTABLE LightedGeometryVTable = {
    PhysxLightedGeometryTestRay,
    PhysxLightedGeometryComputeNormal,
    PhysxLightedGeometryTestBounds,
    PhysxLightedGeometryGetMaterial,
    PhysxLightedGeometryFree
};

//
// Static Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PBRGeometryAllocateImplementation(
    _In_ PCPBR_GEOMETRY_VTABLE PBRGeometryVTable,
    _In_opt_ PPHYSX_AREA_LIGHT_REFERENCE_COUNT ReferenceCount,
    _When_(DataSizeInBytes != 0, _In_reads_bytes_opt_(DataSizeInBytes)) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _When_(DataSizeInBytes != 0, _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && DataSizeInBytes % _Curr_ == 0)) SIZE_T DataAlignment,
    _Out_ PPBR_GEOMETRY *PBRGeometry
    )
{
    ASSERT(PBRGeometryVTable != NULL);
    ASSERT(ReferenceCount == NULL || PBRGeometryVTable == &LightedGeometryVTable);
    ASSERT(DataSizeInBytes == 0 || 
           (Data != NULL && DataAlignment != 0 && 
           (DataAlignment & DataAlignment - 1) == 0 &&
           DataSizeInBytes % DataAlignment == 0));
    ASSERT(PBRGeometry != NULL);

    BOOL AllocationSuccessful;
    PVOID HeaderAllocation;
    PVOID DataAllocation;
    PPBR_GEOMETRY AllocatedGeometry;

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

    if (ReferenceCount != NULL)
    {
        AllocatedGeometry->ReferenceCount.Area = ReferenceCount;
    }
    else
    {
        AllocatedGeometry->ReferenceCount.Simple = 1;
    }

    if (DataSizeInBytes != 0)
    {
        memcpy(DataAllocation, Data, DataSizeInBytes);
    }

    *PBRGeometry = AllocatedGeometry;

    return ISTATUS_SUCCESS;
}

int 
PhysxLightedGeometryCompareLights(
    _In_ const void * Ptr0,
    _In_ const void * Ptr1
    )
{
    PCPHYSX_ATTACHED_LIGHT AttachedLight0;
    PCPHYSX_ATTACHED_LIGHT AttachedLight1;

    AttachedLight0 = (PCPHYSX_ATTACHED_LIGHT) Ptr0;
    AttachedLight1 = (PCPHYSX_ATTACHED_LIGHT) Ptr1;

    if (AttachedLight0->Face < AttachedLight1->Face)
    {
        return -1;
    }

    if (AttachedLight0->Face > AttachedLight1->Face)
    {
        return 1;
    }

    return 0;
}

//
// Internal Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PhysxLightedGeometryAllocate(
    _In_ PCPBR_GEOMETRY_VTABLE PBRGeometryVTable,
    _In_ PPHYSX_AREA_LIGHT_REFERENCE_COUNT ReferenceCount,
    _In_ SIZE_T AttachCount,
    _When_(DataSizeInBytes != 0, _In_reads_bytes_opt_(DataSizeInBytes)) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _When_(DataSizeInBytes != 0, _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && DataSizeInBytes % _Curr_ == 0)) SIZE_T DataAlignment,
    _Out_ PPBR_GEOMETRY *PBRGeometry
    )
{
    PPHYSX_LIGHTED_GEOMETRY AllocatedLightedGeometry;
    PHYSX_LIGHTED_GEOMETRY LightedGeometry;
    PPBR_GEOMETRY NestedGeometry;
    PPBR_GEOMETRY ResultGeometry;
    ISTATUS Status;

    ASSERT(PBRGeometryVTable != NULL);
    ASSERT(ReferenceCount != NULL);
    ASSERT(DataSizeInBytes == 0 || 
           (Data != NULL && DataAlignment != 0 && 
           (DataAlignment & DataAlignment - 1) && 
           DataSizeInBytes % DataAlignment == 0));
    ASSERT(PBRGeometry != NULL);

    if (PHYSX_LIGHTED_GEOMETRY_MAX_ATTACHED_LIGHTS < AttachCount)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    if (AttachCount != 0)
    {
        LightedGeometry.Lights = malloc(sizeof(PHYSX_ATTACHED_LIGHT) * AttachCount);

        if (LightedGeometry.Lights == NULL)
        {
            return ISTATUS_ALLOCATION_FAILED;
        }
    }
    else
    {
        LightedGeometry.Lights = NULL;
    }

    LightedGeometry.NumberOfLights = 0;
    LightedGeometry.Geometry = NULL;

    Status = PBRGeometryAllocateImplementation(&LightedGeometryVTable, 
                                               ReferenceCount,
                                               &LightedGeometry,
                                               sizeof(PHYSX_LIGHTED_GEOMETRY),
                                               _Alignof(PHYSX_LIGHTED_GEOMETRY),
                                               &ResultGeometry);

    if (Status != ISTATUS_SUCCESS)
    {
        ASSERT(Status == ISTATUS_ALLOCATION_FAILED);
        free(LightedGeometry.Lights);
        return Status;
    }

    //
    // The nested geometry must be allocated last. We should never call the
    // destructor of the nested geometry from this routine as it may have 
    // side affects.
    //

    Status = PBRGeometryAllocateImplementation(PBRGeometryVTable, 
                                               NULL,
                                               Data,
                                               DataSizeInBytes,
                                               DataAlignment,
                                               &NestedGeometry);

    if (Status != ISTATUS_SUCCESS)
    {
        ASSERT(Status == ISTATUS_ALLOCATION_FAILED);
        PBRGeometryFree(ResultGeometry);
        return Status;
    }

    AllocatedLightedGeometry = (PPHYSX_LIGHTED_GEOMETRY) ResultGeometry->Data;
    AllocatedLightedGeometry->Geometry = NestedGeometry;

    *PBRGeometry = ResultGeometry;

    return Status;
}

VOID
PhysxLightedGeometryAttachLight(
    _Inout_ PPBR_GEOMETRY LightedGeometry,
    _In_ PPBR_LIGHT Light,
    _In_ UINT32 Face
    )
{
    PPHYSX_LIGHTED_GEOMETRY LightedGeometryData;
    PPHYSX_ATTACHED_LIGHT AttachedLight;

    ASSERT(LightedGeometry->VTable == &LightedGeometryVTable);

    LightedGeometryData = (PPHYSX_LIGHTED_GEOMETRY) LightedGeometry->Data;

    AttachedLight = LightedGeometryData->Lights + LightedGeometryData->NumberOfLights;
    LightedGeometryData->NumberOfLights += 1;

    AttachedLight->Light = Light;
    AttachedLight->Face = Face;
}

VOID
PhysxLightedGeometryFinalize(
    _Inout_ PPBR_GEOMETRY LightedGeometry
    )
{
    PPHYSX_LIGHTED_GEOMETRY LightedGeometryData;
    
    ASSERT(LightedGeometry != NULL);
    
    LightedGeometryData = (PPHYSX_LIGHTED_GEOMETRY) LightedGeometry->Data;
    
    qsort(LightedGeometryData->Lights,
          LightedGeometryData->NumberOfLights,
          sizeof(PHYSX_ATTACHED_LIGHT),
          PhysxLightedGeometryCompareLights);
}

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
    ISTATUS Status;

    if (PBRGeometryVTable == NULL ||
        PBRGeometryVTable == &LightedGeometryVTable)
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

    Status = PBRGeometryAllocateImplementation(PBRGeometryVTable, 
                                               NULL,
                                               Data,
                                               DataSizeInBytes,
                                               DataAlignment,
                                               PBRGeometry);

    return Status;
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
    PCPHYSX_LIGHTED_GEOMETRY LightedGeometry;
    PHYSX_ATTACHED_LIGHT Key;
    PCPHYSX_ATTACHED_LIGHT ResultAttachedLight;
    void* Result;

    if (PBRGeometry == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (Light == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }
    
    if (PBRGeometry->VTable != &LightedGeometryVTable)
    {
        *Light = NULL;
        return ISTATUS_SUCCESS;
    }

    LightedGeometry = (PCPHYSX_LIGHTED_GEOMETRY) PBRGeometry->Data;

    Key.Face = FaceHit;
    Key.Light = NULL;

    Result = bsearch(&Key,
                     LightedGeometry->Lights,
                     LightedGeometry->NumberOfLights,
                     sizeof(PHYSX_ATTACHED_LIGHT),
                     PhysxLightedGeometryCompareLights);

    if (Result == NULL)
    {
        *Light = NULL;
        return ISTATUS_SUCCESS;
    }

    ResultAttachedLight = (PCPHYSX_ATTACHED_LIGHT) Result;
    *Light = ResultAttachedLight->Light;

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

    if (PBRGeometry->VTable == &LightedGeometryVTable)
    {
        AreaLightReferenceCountRetain(PBRGeometry->ReferenceCount.Area);
    }
    else
    {
        PBRGeometry->ReferenceCount.Simple += 1;
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

    if (PBRGeometry->VTable == &LightedGeometryVTable)
    {
        AreaLightReferenceCountRelease(PBRGeometry->ReferenceCount.Area);
    }
    else
    {
        ReferenceCount = (PBRGeometry->ReferenceCount.Simple -= 1);

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