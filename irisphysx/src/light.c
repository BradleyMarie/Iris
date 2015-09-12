/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    light.c

Abstract:

    This file contains the definitions for the LIGHT type.

--*/

#include <irisphysxp.h>

//
// Types
//

struct _LIGHT {
    PCLIGHT_VTABLE VTable;
    SIZE_T ReferenceCount;
    PVOID Data;
};

//
// Functions
//

_Check_return_
_Ret_maybenull_
PLIGHT
LightAllocate(
    _In_ PCLIGHT_VTABLE LightVTable,
    _In_reads_bytes_(DataSizeInBytes) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _In_ SIZE_T DataAlignment
    )
{
    PVOID HeaderAllocation;
    PVOID DataAllocation;
    PLIGHT Light;

    if (LightVTable == NULL)
    {
        return NULL;
    }

    if (Data == NULL && DataSizeInBytes == 0)
    {
        return NULL;
    }

    HeaderAllocation = IrisAlignedMallocWithHeader(sizeof(LIGHT),
                                                   sizeof(PVOID),
                                                   DataSizeInBytes,
                                                   DataAlignment,
                                                   &DataAllocation);

    if (HeaderAllocation == NULL)
    {
        return NULL;
    }

    Light = (PLIGHT) HeaderAllocation;

    Light->VTable = LightVTable;
    Light->Data = DataAllocation;
    Light->ReferenceCount = 1;

    if (DataSizeInBytes != 0)
    {
        memcpy(DataAllocation, Data, DataSizeInBytes);
    }

    return Light;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
LightSample(
    _In_ PCLIGHT Light,
    _In_ POINT3 HitPoint,
    _Inout_ PVISIBILITY_TESTER VisibilityTester,
    _Inout_ PRANDOM Rng,
    _Inout_ PSPECTRUM_COMPOSITOR Compositor,
    _Out_ PCSPECTRUM *Spectrum,
    _Out_ PVECTOR3 ToLight,
    _Out_ PFLOAT Pdf
    )
{
    ISTATUS Status;

    if (Light == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (PointValidate(HitPoint) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (VisibilityTester == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (Rng == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (Compositor == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    if (Spectrum == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }

    if (ToLight == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_06;
    }

    if (Pdf == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_07;
    }

    Status = Light->VTable->SampleRoutine(Light->Data,
                                          HitPoint,
                                          VisibilityTester,
                                          Rng,
                                          Compositor, 
                                          Spectrum,
                                          ToLight,
                                          Pdf);

    return Status;
}

VOID
LightReference(
    _In_opt_ PLIGHT Light
    )
{
    if (Light == NULL)
    {
        return;
    }

    Light->ReferenceCount += 1;
}

VOID
LightDereference(
    _In_opt_ _Post_invalid_ PLIGHT Light
    )
{
    PFREE_ROUTINE FreeRoutine;

    if (Light == NULL)
    {
        return;
    }

    Light->ReferenceCount -= 1;

    if (Light->ReferenceCount == 0)
    {
        FreeRoutine = Light->VTable->FreeRoutine;

        if (FreeRoutine != NULL)
        {
            FreeRoutine(Light->Data);
        }

        IrisAlignedFree(Light);
    }
}