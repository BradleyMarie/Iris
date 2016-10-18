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
    PPHYSX_AREA_LIGHT_REFERENCE_COUNT Area;
    SIZE_T Simple;
} PHYSX_GEOMETRY_SHARED_REFERENCE_COUNT;

struct _PHYSX_GEOMETRY {
    PCPHYSX_GEOMETRY_VTABLE VTable;
    PHYSX_GEOMETRY_SHARED_REFERENCE_COUNT ReferenceCount;
    PVOID Data;
};

//
// Internal Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PhysxGeometryAllocateInternal(
    _In_ PCPHYSX_GEOMETRY_VTABLE GeometryVTable,
    _In_ PPHYSX_AREA_LIGHT_REFERENCE_COUNT ReferenceCount,
    _When_(DataSizeInBytes != 0, _In_reads_bytes_opt_(DataSizeInBytes)) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _When_(DataSizeInBytes != 0, _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && DataSizeInBytes % _Curr_ == 0)) SIZE_T DataAlignment,
    _Out_ PPHYSX_GEOMETRY *Geometry
    )
{
    ASSERT(GeometryVTable != NULL);
    ASSERT(ReferenceCount == NULL || GeometryVTable == &LightedGeometryAdapterVTable);
    ASSERT(DataSizeInBytes == 0 || 
           (Data != NULL && DataAlignment != 0 && 
           (DataAlignment & DataAlignment - 1) == 0 &&
           DataSizeInBytes % DataAlignment == 0));
    ASSERT(Geometry != NULL);

    BOOL AllocationSuccessful;
    PVOID HeaderAllocation;
    PVOID DataAllocation;
    PPHYSX_GEOMETRY AllocatedGeometry;

    AllocationSuccessful = IrisAlignedAllocWithHeader(sizeof(PHYSX_GEOMETRY),
                                                      _Alignof(PHYSX_GEOMETRY),
                                                      &HeaderAllocation,
                                                      DataSizeInBytes,
                                                      DataAlignment,
                                                      &DataAllocation,
                                                      NULL);

    if (AllocationSuccessful == FALSE)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    AllocatedGeometry = (PPHYSX_GEOMETRY) HeaderAllocation;

    AllocatedGeometry->VTable = GeometryVTable;
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

    *Geometry = AllocatedGeometry;

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PhysxGeometryTestRayAdapter(
    _In_ PCVOID Context,
    _In_ RAY Ray,
    _In_ PHIT_ALLOCATOR HitAllocator,
    _Out_ PHIT_LIST *HitList
    )
{
    PCPHYSX_GEOMETRY Geometry;
    PHYSX_HIT_ALLOCATOR PhysxHitAllocator;
    ISTATUS Status;
    
    ASSERT(Context != NULL);
    ASSERT(RayValidate(Ray) != FALSE);
    ASSERT(HitAllocator != NULL);
    ASSERT(HitList != NULL);
    
    Geometry = (PCPHYSX_GEOMETRY) Context;
    
    PhysxHitAllocatorInitialize(&PhysxHitAllocator,
                                HitAllocator,
                                Geometry,
                                Ray);
    
    Status = Geometry->VTable->TestRayRoutine(Geometry->Data,
                                              Ray,
                                              &PhysxHitAllocator,
                                              HitList);

    return Status;
}

_Ret_
PCPHYSX_GEOMETRY_VTABLE
PhysxGeometryGetVTable(
    _In_ PCPHYSX_GEOMETRY Geometry
    )
{
    ASSERT(Geometry != NULL);
    return Geometry->VTable;
}

_Ret_
PVOID
PhysxGeometryGetMutableData(
    _In_ PCPHYSX_GEOMETRY Geometry
    )
{
    ASSERT(Geometry != NULL);
    return Geometry->Data;
}

_Ret_
PCVOID
PhysxGeometryGetData(
    _In_ PCPHYSX_GEOMETRY Geometry
    )
{
    ASSERT(Geometry != NULL);
    return Geometry->Data;
}

VOID
PhysxGeometryFree(
    _In_ _Post_invalid_ PPHYSX_GEOMETRY Geometry
    )
{
    PFREE_ROUTINE FreeRoutine;

    ASSERT(Geometry != NULL);
    
    FreeRoutine = Geometry->VTable->FreeRoutine;

    if (FreeRoutine != NULL)
    {
        FreeRoutine(Geometry->Data);
    }

    IrisAlignedFree(Geometry);
}

//
// Public Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PhysxGeometryAllocate(
    _In_ PCPHYSX_GEOMETRY_VTABLE GeometryVTable,
    _When_(DataSizeInBytes != 0, _In_reads_bytes_opt_(DataSizeInBytes)) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _When_(DataSizeInBytes != 0, _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && DataSizeInBytes % _Curr_ == 0)) SIZE_T DataAlignment,
    _Out_ PPHYSX_GEOMETRY *PBRGeometry
    )
{
    ISTATUS Status;

    if (GeometryVTable == NULL ||
        GeometryVTable == &LightedGeometryAdapterVTable)
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

    Status = PhysxGeometryAllocateInternal(GeometryVTable,
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
    _In_ PCPHYSX_GEOMETRY Geometry,
    _In_ PPHYSX_HIT_ALLOCATOR HitAllocator,
    _Out_ PHIT_LIST *HitList
    )
{
    PCPHYSX_GEOMETRY OldGeometry;
    ISTATUS Status;
    RAY Ray;
    
    if (Geometry == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }
    
    if (HitAllocator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (HitList == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }
    
    Ray = PhysxHitAllocatorGetRay(HitAllocator);
    
    OldGeometry = PhysxHitAllocatorGetGeometry(HitAllocator);
    PhysxHitAllocatorSetGeometry(HitAllocator, Geometry);
    
    Status = Geometry->VTable->TestRayRoutine(Geometry->Data,
                                              Ray,
                                              HitAllocator,
                                              HitList);

    PhysxHitAllocatorSetGeometry(HitAllocator, OldGeometry);

    return Status;
}
    
_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS 
PhysxGeometryCheckBounds(
    _In_ PCPHYSX_GEOMETRY Geometry,
    _In_ PCMATRIX ModelToWorld,
    _In_ BOUNDING_BOX WorldAlignedBoundingBox,
    _Out_ PBOOL IsInsideBox
    )
{
    ISTATUS Status;
    
    if (Geometry == NULL)
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
    
    Status = Geometry->VTable->CheckBoundsRoutine(Geometry->Data,
                                                  ModelToWorld,
                                                  WorldAlignedBoundingBox,
                                                  IsInsideBox);

    return Status;
}
    
_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PhysxGeometryComputeNormal(
    _In_ PCPHYSX_GEOMETRY Geometry,
    _In_ POINT3 ModelHitPoint,
    _In_ UINT32 FaceHit,
    _Out_ PVECTOR3 ModelSurfaceNormal
    )
{
    ISTATUS Status;
    
    if (Geometry == NULL)
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
    
    Status = Geometry->VTable->ComputeNormalRoutine(Geometry->Data,
                                                    ModelHitPoint,
                                                    FaceHit,
                                                    ModelSurfaceNormal);

    return Status;
}
    
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PhysxGeometryGetMaterial(
    _In_ PCPHYSX_GEOMETRY Geometry,
    _In_ UINT32 FaceHit,
    _Outptr_result_maybenull_ PCPHYSX_MATERIAL *Material
    )
{
    ISTATUS Status;
    
    if (Geometry == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (Material == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }
    
    Status = Geometry->VTable->GetMaterialRoutine(Geometry->Data,
                                                  FaceHit,
                                                  Material);

    return Status;
}
    
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PhysxGeometryGetLight(
    _In_ PCPHYSX_GEOMETRY Geometry,
    _In_ UINT32 Face,
    _Outptr_result_maybenull_ PCPHYSX_LIGHT *Light
    )
{
    ISTATUS Status;

    if (Geometry == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (Light == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }
    
    if (Geometry->VTable != &LightedGeometryAdapterVTable)
    {
        *Light = NULL;
        return ISTATUS_SUCCESS;
    }

    Status = PhysxLightedGeometryAdapterGetLight(Geometry,
                                                 Face,
                                                 Light);
    
    return Status;
}
    
VOID
PhysxGeometryRetain(
    _In_opt_ PPHYSX_GEOMETRY Geometry
    )
{
    if (Geometry == NULL)
    {
        return;
    }

    if (Geometry->VTable == &LightedGeometryAdapterVTable)
    {
        AreaLightReferenceCountRetain(Geometry->ReferenceCount.Area);
    }
    else
    {
        Geometry->ReferenceCount.Simple += 1;
    }
}
    
VOID
PhysxGeometryRelease(
    _In_opt_ _Post_invalid_ PPHYSX_GEOMETRY Geometry
    )
{
    SIZE_T ReferenceCount;

    if (Geometry == NULL)
    {
        return;
    }

    if (Geometry->VTable == &LightedGeometryAdapterVTable)
    {
        AreaLightReferenceCountRelease(Geometry->ReferenceCount.Area);
    }
    else
    {
        ReferenceCount = (Geometry->ReferenceCount.Simple -= 1);

        if (ReferenceCount == 0)
        {
            PhysxGeometryFree(Geometry);
        }
    }
}
