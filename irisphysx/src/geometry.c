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

struct _PBR_GEOMETRY {
    PCPBR_GEOMETRY_VTABLE VTable;
    SIZE_T ReferenceCount;
    PVOID Data;
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
    AllocatedGeometry->ReferenceCount = 1;

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
    _Out_ PVECTOR3 SurfaceNormal
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

    if (SurfaceNormal == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }
    
    Status = PBRGeometry->VTable->ComputeNormalRoutine(PBRGeometry->Data,
                                                       ModelHitPoint,
                                                       FaceHit,
                                                       SurfaceNormal);

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
    ISTATUS Status;
    
    if (PBRGeometry == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (Light == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }
    
    Status = PBRGeometry->VTable->GetLightRoutine(PBRGeometry->Data,
                                                  FaceHit,
                                                  Light);

    return Status;
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

    PBRGeometry->ReferenceCount += 1;
}
    
VOID
PBRGeometryRelease(
    _In_opt_ _Post_invalid_ PPBR_GEOMETRY PBRGeometry
    )
{
    PFREE_ROUTINE FreeRoutine;

    if (PBRGeometry == NULL)
    {
        return;
    }

    PBRGeometry->ReferenceCount -= 1;

    if (PBRGeometry->ReferenceCount == 0)
    {
        FreeRoutine = PBRGeometry->VTable->FreeRoutine;

        if (FreeRoutine != NULL)
        {
            FreeRoutine(PBRGeometry->Data);
        }

        IrisAlignedFree(PBRGeometry);
    }
}