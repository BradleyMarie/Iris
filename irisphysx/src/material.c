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
ISTATUS
MaterialSample(
    _In_ PCMATERIAL Material,
    _In_ POINT3 WorldHitPoint,
    _In_ VECTOR3 WorldViewer,
    _In_ PCVECTOR3 WorldNormalizedNormal,
    _In_opt_ PCVOID AdditionalData,
    _Inout_ PRANDOM Rng,
    _Inout_ PSPECTRUM_COMPOSITOR Compositor,
    _Out_ PCSPECTRUM *Output
    )
{
    ISTATUS Status;
    
    Status = Material->VTable->SampleRoutine(Material->Data,
                                             WorldHitPoint,
                                             WorldViewer,
                                             WorldNormalizedNormal,
                                             AdditionalData,
                                             Rng,
                                             Compositor,
                                             Output);

    return Status;
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