/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    direct.c

Abstract:

    This file contains the definitions for the DIRECT_LIGHT type.

--*/

#include <irisphysxp.h>

//
// Types
//

struct _DIRECT_LIGHT {
    PCDIRECT_LIGHT_VTABLE VTable;
    SIZE_T ReferenceCount;
    PVOID Data;
};

//
// Functions
//

_Check_return_
_Ret_maybenull_
PDIRECT_LIGHT
DirectLightAllocate(
    _In_ PCDIRECT_LIGHT_VTABLE DirectLightVTable,
    _In_reads_bytes_(DataSizeInBytes) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _In_ SIZE_T DataAlignment
    )
{
    PVOID HeaderAllocation;
    PVOID DataAllocation;
    PDIRECT_LIGHT DirectLight;

    if (DirectLightVTable == NULL)
    {
        return NULL;
    }

    if (Data == NULL && DataSizeInBytes == 0)
    {
        return NULL;
    }

    HeaderAllocation = IrisAlignedMallocWithHeader(sizeof(DIRECT_LIGHT),
                                                   sizeof(PVOID),
                                                   DataSizeInBytes,
                                                   DataAlignment,
                                                   &DataAllocation);

    if (HeaderAllocation == NULL)
    {
        return NULL;
    }

    DirectLight = (PDIRECT_LIGHT) HeaderAllocation;

    DirectLight->VTable = DirectLightVTable;
    DirectLight->Data = DataAllocation;
    DirectLight->ReferenceCount = 1;

    if (DataSizeInBytes != 0)
    {
        memcpy(DataAllocation, Data, DataSizeInBytes);
    }

    return DirectLight;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
DirectLightSample(
    _In_ PCDIRECT_LIGHT DirectLight,
    _In_ POINT3 WorldHitPoint,
    _In_ VECTOR3 WorldViewer,
    _In_ PCVECTOR3 WorldNormalizedNormal,
    _In_opt_ PCVOID AdditionalData,
    _Inout_ PRANDOM Rng,
    _Inout_ PVISIBILITY_TESTER VisibilityTester,
    _Inout_ PSPECTRUM_COMPOSITOR Compositor,
    _Out_ PCSPECTRUM *Output
    )
{
    ISTATUS Status;
    
    Status = DirectLight->VTable->SampleRoutine(DirectLight->Data,
                                                WorldHitPoint,
                                                WorldViewer,
                                                WorldNormalizedNormal,
                                                AdditionalData,
                                                Rng,
                                                VisibilityTester,
                                                Compositor,
                                                Output);

    return Status;
}

VOID
DirectLightReference(
    _In_opt_ PDIRECT_LIGHT DirectLight
    )
{
    if (DirectLight == NULL)
    {
        return;
    }

    DirectLight->ReferenceCount += 1;
}

VOID
DirectLightDereference(
    _In_opt_ _Post_invalid_ PDIRECT_LIGHT DirectLight
    )
{
    PFREE_ROUTINE FreeRoutine;

    if (DirectLight == NULL)
    {
        return;
    }

    DirectLight->ReferenceCount -= 1;

    if (DirectLight->ReferenceCount == 0)
    {
        FreeRoutine = DirectLight->VTable->FreeRoutine;

        if (FreeRoutine != NULL)
        {
            FreeRoutine(DirectLight->Data);
        }

        IrisAlignedFree(DirectLight);
    }
}