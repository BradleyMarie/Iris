/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    indirect.c

Abstract:

    This file contains the definitions for the INDIRECT_LIGHT type.

--*/

#include <irisphysxp.h>

//
// Types
//

struct _INDIRECT_LIGHT {
    PCINDIRECT_LIGHT_VTABLE VTable;
    SIZE_T ReferenceCount;
    PVOID Data;
};

//
// Functions
//

_Check_return_
_Ret_maybenull_
PINDIRECT_LIGHT
IndirectLightAllocate(
    _In_ PCINDIRECT_LIGHT_VTABLE IndirectLightVTable,
    _In_reads_bytes_(DataSizeInBytes) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _In_ SIZE_T DataAlignment
    )
{
    PVOID HeaderAllocation;
    PVOID DataAllocation;
    PINDIRECT_LIGHT IndirectLight;

    if (IndirectLightVTable == NULL)
    {
        return NULL;
    }

    if (Data == NULL && DataSizeInBytes == 0)
    {
        return NULL;
    }

    HeaderAllocation = IrisAlignedMallocWithHeader(sizeof(INDIRECT_LIGHT),
                                                   sizeof(PVOID),
                                                   DataSizeInBytes,
                                                   DataAlignment,
                                                   &DataAllocation);

    if (HeaderAllocation == NULL)
    {
        return NULL;
    }

    IndirectLight = (PINDIRECT_LIGHT) HeaderAllocation;

    IndirectLight->VTable = IndirectLightVTable;
    IndirectLight->Data = DataAllocation;
    IndirectLight->ReferenceCount = 1;

    if (DataSizeInBytes != 0)
    {
        memcpy(DataAllocation, Data, DataSizeInBytes);
    }

    return IndirectLight;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
IndirectLightSample(
    _In_ PCINDIRECT_LIGHT IndirectLight,
    _In_ POINT3 WorldHitPoint,
    _In_ VECTOR3 WorldViewer,
    _In_ PCVECTOR3 WorldNormalizedNormal,
    _In_opt_ PCVOID AdditionalData,
    _Inout_ PRANDOM Rng,
    _Inout_ PVOID RayTracer,
    _Inout_ PSPECTRUM_COMPOSITOR Compositor,
    _Out_ PCSPECTRUM *Output
    )
{
    ISTATUS Status;
    
    Status = IndirectLight->VTable->SampleRoutine(IndirectLight->Data,
                                                  WorldHitPoint,
                                                  WorldViewer,
                                                  WorldNormalizedNormal,
                                                  AdditionalData,
                                                  Rng,
                                                  RayTracer,
                                                  Compositor,
                                                  Output);

    return Status;
}

VOID
IndirectLightReference(
    _In_opt_ PINDIRECT_LIGHT IndirectLight
    )
{
    if (IndirectLight == NULL)
    {
        return;
    }

    IndirectLight->ReferenceCount += 1;
}

VOID
IndirectLightDereference(
    _In_opt_ _Post_invalid_ PINDIRECT_LIGHT IndirectLight
    )
{
    PFREE_ROUTINE FreeRoutine;

    if (IndirectLight == NULL)
    {
        return;
    }

    IndirectLight->ReferenceCount -= 1;

    if (IndirectLight->ReferenceCount == 0)
    {
        FreeRoutine = IndirectLight->VTable->FreeRoutine;

        if (FreeRoutine != NULL)
        {
            FreeRoutine(IndirectLight->Data);
        }

        IrisAlignedFree(IndirectLight);
    }
}