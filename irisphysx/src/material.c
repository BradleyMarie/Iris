/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    material.c

Abstract:

    This file contains the definitions for the MATERIAL type.

--*/

#include <irisphysxp.h>

//
// Types
//

struct _MATERIAL {
    PCMATERIAL_VTABLE VTable;
    SIZE_T ReferenceCount;
    PVOID Data;
};

//
// Functions
//

_Check_return_
_Ret_maybenull_
PMATERIAL
MaterialAllocate(
    _In_ PCMATERIAL_VTABLE MaterialVTable,
    _In_reads_bytes_(DataSizeInBytes) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _In_ SIZE_T DataAlignment
    )
{
    PVOID HeaderAllocation;
    PVOID DataAllocation;
    PMATERIAL Material;

    if (MaterialVTable == NULL)
    {
        return NULL;
    }

    if (Data == NULL && DataSizeInBytes == 0)
    {
        return NULL;
    }

    HeaderAllocation = IrisAlignedMallocWithHeader(sizeof(MATERIAL),
                                                   sizeof(PVOID),
                                                   DataSizeInBytes,
                                                   DataAlignment,
                                                   &DataAllocation);

    if (HeaderAllocation == NULL)
    {
        return NULL;
    }

    Material = (PMATERIAL) HeaderAllocation;

    Material->VTable = MaterialVTable;
    Material->Data = DataAllocation;
    Material->ReferenceCount = 1;

    if (DataSizeInBytes != 0)
    {
        memcpy(DataAllocation, Data, DataSizeInBytes);
    }

    return Material;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
MaterialSample(
    _In_ PCMATERIAL Material,
    _In_ POINT3 ModelHitPoint,
    _In_opt_ PCVOID AdditionalData,
    _In_ VECTOR3 ShapeSurfaceNormal,
    _In_opt_ PCMATRIX ModelToWorld,
    _Inout_ PBRDF_ALLOCATOR BrdfAllocator,
    _Out_ PBRDF *Brdf
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
    
    if (VectorValidate(ShapeSurfaceNormal) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }
    
    if (BrdfAllocator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }
    
    if (Brdf == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_06;
    }
    
    Status = Material->VTable->SampleRoutine(Material->Data,
                                             ModelHitPoint,
                                             AdditionalData,
                                             ShapeSurfaceNormal,
                                             ModelToWorld,
                                             BrdfAllocator,
                                             Brdf);
    
    return ISTATUS_SUCCESS;
}

VOID
MaterialReference(
    _In_opt_ PMATERIAL Material
    )
{
    if (Material == NULL)
    {
        return;
    }

    Material->ReferenceCount += 1;
}

VOID
MaterialDereference(
    _In_opt_ _Post_invalid_ PMATERIAL Material
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