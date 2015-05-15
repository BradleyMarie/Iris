/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    emissive.c

Abstract:

    This file contains the definitions for the EMISSIVE_LIGHT type.

--*/

#include <irisphysxp.h>

//
// Types
//

struct _EMISSIVE_LIGHT {
    PCEMISSIVE_LIGHT_VTABLE VTable;
    SIZE_T ReferenceCount;
    PVOID Data;
};

//
// Functions
//

_Check_return_
_Ret_maybenull_
PEMISSIVE_LIGHT
EmissiveLightAllocate(
    _In_ PCEMISSIVE_LIGHT_VTABLE EmissiveLightVTable,
    _In_reads_bytes_(DataSizeInBytes) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _In_ SIZE_T DataAlignment
    )
{
    PVOID HeaderAllocation;
    PVOID DataAllocation;
    PEMISSIVE_LIGHT EmissiveLight;

    if (EmissiveLightVTable == NULL)
    {
        return NULL;
    }

    if (Data == NULL && DataSizeInBytes == 0)
    {
        return NULL;
    }

    HeaderAllocation = IrisAlignedMallocWithHeader(sizeof(EMISSIVE_LIGHT),
                                                   sizeof(PVOID),
                                                   DataSizeInBytes,
                                                   DataAlignment,
                                                   &DataAllocation);

    if (HeaderAllocation == NULL)
    {
        return NULL;
    }

    EmissiveLight = (PEMISSIVE_LIGHT) HeaderAllocation;

    EmissiveLight->VTable = EmissiveLightVTable;
    EmissiveLight->Data = DataAllocation;
    EmissiveLight->ReferenceCount = 1;

    if (DataSizeInBytes != 0)
    {
        memcpy(DataAllocation, Data, DataSizeInBytes);
    }

    return EmissiveLight;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
EmissiveLightSample(
    _In_ PCEMISSIVE_LIGHT EmissiveLight,
    _In_ POINT3 WorldHitPoint,
    _In_ VECTOR3 WorldViewer,
    _In_ PCVECTOR3 WorldNormalizedNormal,
    _In_opt_ PCVOID AdditionalData,
    _Inout_ PRANDOM Rng,
    _Out_ PCSPECTRUM *Output
    )
{
    ISTATUS Status;
    
    Status = EmissiveLight->VTable->SampleRoutine(EmissiveLight->Data,
                                                  WorldHitPoint,
                                                  WorldViewer,
                                                  WorldNormalizedNormal,
                                                  AdditionalData,
                                                  Rng,
                                                  Output);

    return Status;
}

VOID
EmissiveLightReference(
    _In_opt_ PEMISSIVE_LIGHT EmissiveLight
    )
{
    if (EmissiveLight == NULL)
    {
        return;
    }

    EmissiveLight->ReferenceCount += 1;
}

VOID
EmissiveLightDereference(
    _In_opt_ _Post_invalid_ PEMISSIVE_LIGHT EmissiveLight
    )
{
    PFREE_ROUTINE FreeRoutine;

    if (EmissiveLight == NULL)
    {
        return;
    }

    EmissiveLight->ReferenceCount -= 1;

    if (EmissiveLight->ReferenceCount == 0)
    {
        FreeRoutine = EmissiveLight->VTable->FreeRoutine;

        if (FreeRoutine != NULL)
        {
            FreeRoutine(EmissiveLight->Data);
        }

        IrisAlignedFree(EmissiveLight);
    }
}