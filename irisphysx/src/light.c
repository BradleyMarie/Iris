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
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
LightAllocate(
    _In_ PCLIGHT_VTABLE LightVTable,
    _When_(DataSizeInBytes != 0, _In_reads_bytes_opt_(DataSizeInBytes)) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _When_(DataSizeInBytes != 0, _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && DataSizeInBytes % _Curr_ == 0)) SIZE_T DataAlignment,
    _Out_ PLIGHT *Light
    )
{
    BOOL AllocationSuccessful;
    PVOID HeaderAllocation;
    PVOID DataAllocation;
    PLIGHT AllocatedLight;

    if (LightVTable == NULL)
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

    if (Light == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    AllocationSuccessful = IrisAlignedAllocWithHeader(sizeof(LIGHT),
                                                      _Alignof(LIGHT),
                                                      &HeaderAllocation,
                                                      DataSizeInBytes,
                                                      DataAlignment,
                                                      &DataAllocation,
                                                      NULL);

    if (AllocationSuccessful == FALSE)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    AllocatedLight = (PLIGHT) HeaderAllocation;

    AllocatedLight->VTable = LightVTable;
    AllocatedLight->Data = DataAllocation;
    AllocatedLight->ReferenceCount = 1;

    if (DataSizeInBytes != 0)
    {
        memcpy(DataAllocation, Data, DataSizeInBytes);
    }

    *Light = AllocatedLight;

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
LightSample(
    _In_ PCLIGHT Light,
    _In_ POINT3 HitPoint,
    _Inout_ PSPECTRUM_VISIBILITY_TESTER VisibilityTester,
    _Inout_ PRANDOM_REFERENCE Rng,
    _Inout_ PSPECTRUM_COMPOSITOR_REFERENCE Compositor,
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

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
LightComputeEmissive(
    _In_ PCLIGHT Light,
    _In_ RAY ToLight,
    _Inout_ PSPECTRUM_VISIBILITY_TESTER Tester,
    _Out_ PCSPECTRUM *Spectrum
    )
{
    ISTATUS Status;

    if (Light == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (RayValidate(ToLight) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (Tester == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (Spectrum == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    Status = Light->VTable->ComputeEmissiveRoutine(Light->Data,
                                                   ToLight,
                                                   Tester,
                                                   Spectrum);

    return Status;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
LightComputeEmissiveWithPdf(
    _In_ PCLIGHT Light,
    _In_ RAY ToLight,
    _Inout_ PSPECTRUM_VISIBILITY_TESTER Tester,
    _Out_ PCSPECTRUM *Spectrum,
    _Out_ PFLOAT Pdf
    )
{
    ISTATUS Status;

    if (Light == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (RayValidate(ToLight) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (Tester == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (Spectrum == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (Pdf == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    Status = Light->VTable->ComputeEmissiveWithPdfRoutine(Light->Data,
                                                          ToLight,
                                                          Tester,
                                                          Spectrum,
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