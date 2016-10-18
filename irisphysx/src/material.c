/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    material.c

Abstract:

    This file contains the definitions for the PHYSX_MATERIAL type.

--*/

#include <irisphysxp.h>

//
// Types
//

struct _PHYSX_MATERIAL {
    PCPHYSX_MATERIAL_VTABLE VTable;
    SIZE_T ReferenceCount;
    PVOID Data;
};

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PhysxMaterialAllocate(
    _In_ PCPHYSX_MATERIAL_VTABLE MaterialVTable,
    _When_(DataSizeInBytes != 0, _In_reads_bytes_opt_(DataSizeInBytes)) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _When_(DataSizeInBytes != 0, _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && DataSizeInBytes % _Curr_ == 0)) SIZE_T DataAlignment,
    _Out_ PPHYSX_MATERIAL *Material
    )
{
    BOOL AllocationSuccessful;
    PVOID HeaderAllocation;
    PVOID DataAllocation;
    PPHYSX_MATERIAL AllocatedMaterial;

    if (MaterialVTable == NULL)
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

    if (Material == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    AllocationSuccessful = IrisAlignedAllocWithHeader(sizeof(PHYSX_MATERIAL),
                                                      _Alignof(PHYSX_MATERIAL),
                                                      &HeaderAllocation,
                                                      DataSizeInBytes,
                                                      DataAlignment,
                                                      &DataAllocation,
                                                      NULL);

    if (AllocationSuccessful == FALSE)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    AllocatedMaterial = (PPHYSX_MATERIAL) HeaderAllocation;

    AllocatedMaterial->VTable = MaterialVTable;
    AllocatedMaterial->Data = DataAllocation;
    AllocatedMaterial->ReferenceCount = 1;

    if (DataSizeInBytes != 0)
    {
        memcpy(DataAllocation, Data, DataSizeInBytes);
    }

    *Material = AllocatedMaterial;

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
PhysxMaterialSample(
    _In_ PCPHYSX_MATERIAL Material,
    _In_ POINT3 ModelHitPoint,
    _In_ VECTOR3 ModelSurfaceNormal,
    _In_ VECTOR3 WorldSurfaceNormal,
    _In_opt_ PCVOID AdditionalData,
    _In_opt_ PCMATRIX ModelToWorld,
    _Inout_ PPHYSX_BRDF_ALLOCATOR Allocator,
    _Out_ PVECTOR3 WorldShadingNormal,
    _Out_ PCPHYSX_BRDF *Brdf
    )
{
    ISTATUS Status;
    
    if (Material == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }
    
    if (PointValidate(ModelHitPoint) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }
    
    if (VectorValidate(ModelSurfaceNormal) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }
    
    if (VectorValidate(WorldSurfaceNormal) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }
    
    if (Allocator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_06;
    }

    if (WorldShadingNormal == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_07;
    }
    
    if (Brdf == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_08;
    }
    
    Status = Material->VTable->SampleRoutine(Material->Data,
                                             ModelHitPoint,
                                             ModelSurfaceNormal,
                                             WorldSurfaceNormal, 
                                             AdditionalData,
                                             ModelToWorld,
                                             Allocator,
                                             WorldShadingNormal,
                                             Brdf);
    
    return Status;
}

VOID
PhysxMaterialRetain(
    _In_opt_ PPHYSX_MATERIAL Material
    )
{
    if (Material == NULL)
    {
        return;
    }

    Material->ReferenceCount += 1;
}

VOID
PhysxMaterialRelease(
    _In_opt_ _Post_invalid_ PPHYSX_MATERIAL Material
    )
{
    PFREE_ROUTINE FreeRoutine;

    if (Material == NULL)
    {
        return;
    }

    Material->ReferenceCount -= 1;

    if (Material->ReferenceCount == 0)
    {
        FreeRoutine = Material->VTable->FreeRoutine;

        if (FreeRoutine != NULL)
        {
            FreeRoutine(Material->Data);
        }

        IrisAlignedFree(Material);
    }
}
