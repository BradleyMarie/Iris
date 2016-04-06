/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    material.c

Abstract:

    This file contains the definitions for the PBR_MATERIAL type.

--*/

#include <irisphysxp.h>

//
// Types
//

struct _PBR_MATERIAL {
    PCPBR_MATERIAL_VTABLE VTable;
    SIZE_T ReferenceCount;
    PVOID Data;
};

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PbrMaterialAllocate(
    _In_ PCPBR_MATERIAL_VTABLE PbrMaterialVTable,
    _When_(DataSizeInBytes != 0, _In_reads_bytes_opt_(DataSizeInBytes)) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _When_(DataSizeInBytes != 0, _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && DataSizeInBytes % _Curr_ == 0)) SIZE_T DataAlignment,
    _Out_ PPBR_MATERIAL *PbrMaterial
    )
{
    BOOL AllocationSuccessful;
    PVOID HeaderAllocation;
    PVOID DataAllocation;
    PPBR_MATERIAL AllocatedPbrMaterial;

    if (PbrMaterialVTable == NULL)
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

    if (PbrMaterial == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    AllocationSuccessful = IrisAlignedAllocWithHeader(sizeof(PBR_MATERIAL),
                                                      _Alignof(PBR_MATERIAL),
                                                      &HeaderAllocation,
                                                      DataSizeInBytes,
                                                      DataAlignment,
                                                      &DataAllocation,
                                                      NULL);

    if (AllocationSuccessful == FALSE)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    AllocatedPbrMaterial = (PPBR_MATERIAL) HeaderAllocation;

    AllocatedPbrMaterial->VTable = PbrMaterialVTable;
    AllocatedPbrMaterial->Data = DataAllocation;
    AllocatedPbrMaterial->ReferenceCount = 1;

    if (DataSizeInBytes != 0)
    {
        memcpy(DataAllocation, Data, DataSizeInBytes);
    }

    *PbrMaterial = AllocatedPbrMaterial;

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
PbrMaterialSample(
    _In_ PCPBR_MATERIAL PbrMaterial,
    _In_ POINT3 ModelHitPoint,
    _In_opt_ PCVOID AdditionalData,
    _In_ VECTOR3 SurfaceNormal,
    _In_opt_ PCMATRIX ModelToWorld,
    _Inout_ PPBR_BRDF_ALLOCATOR BrdfAllocator,
    _Out_ PCPBR_BRDF *PbrBrdf
    )
{
    ISTATUS Status;
    
    if (PbrMaterial == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }
    
    if (PointValidate(ModelHitPoint) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }
    
    if (VectorValidate(SurfaceNormal) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }
    
    if (BrdfAllocator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }
    
    if (PbrBrdf == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_06;
    }
    
    Status = PbrMaterial->VTable->SampleRoutine(PbrMaterial->Data,
                                                ModelHitPoint,
                                                AdditionalData,
                                                SurfaceNormal,
                                                ModelToWorld,
                                                BrdfAllocator,
                                                PbrBrdf);
    
    return ISTATUS_SUCCESS;
}

VOID
PbrMaterialRetain(
    _In_opt_ PPBR_MATERIAL PbrMaterial
    )
{
    if (PbrMaterial == NULL)
    {
        return;
    }

    PbrMaterial->ReferenceCount += 1;
}

VOID
PbrMaterialRelease(
    _In_opt_ _Post_invalid_ PPBR_MATERIAL PbrMaterial
    )
{
    PFREE_ROUTINE FreeRoutine;

    if (PbrMaterial == NULL)
    {
        return;
    }

    PbrMaterial->ReferenceCount -= 1;

    if (PbrMaterial->ReferenceCount == 0)
    {
        FreeRoutine = PbrMaterial->VTable->FreeRoutine;

        if (FreeRoutine != NULL)
        {
            FreeRoutine(PbrMaterial->Data);
        }

        IrisAlignedFree(PbrMaterial);
    }
}