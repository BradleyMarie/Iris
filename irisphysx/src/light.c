/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    light.c

Abstract:

    This file contains the definitions for the PBR_LIGHT type.

--*/

#include <irisphysxp.h>

//
// Types
//

struct _PBR_LIGHT {
    PCPBR_LIGHT_VTABLE VTable;
    SIZE_T ReferenceCount;
    PVOID Data;
};

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PbrLightAllocate(
    _In_ PCPBR_LIGHT_VTABLE PbrLightVTable,
    _When_(DataSizeInBytes != 0, _In_reads_bytes_opt_(DataSizeInBytes)) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _When_(DataSizeInBytes != 0, _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && DataSizeInBytes % _Curr_ == 0)) SIZE_T DataAlignment,
    _Out_ PPBR_LIGHT *PbrLight
    )
{
    BOOL AllocationSuccessful;
    PVOID HeaderAllocation;
    PVOID DataAllocation;
    PPBR_LIGHT AllocatedPbrLight;

    if (PbrLightVTable == NULL)
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

    if (PbrLight == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    AllocationSuccessful = IrisAlignedAllocWithHeader(sizeof(PBR_LIGHT),
                                                      _Alignof(PBR_LIGHT),
                                                      &HeaderAllocation,
                                                      DataSizeInBytes,
                                                      DataAlignment,
                                                      &DataAllocation,
                                                      NULL);

    if (AllocationSuccessful == FALSE)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    AllocatedPbrLight = (PPBR_LIGHT) HeaderAllocation;

    AllocatedPbrLight->VTable = PbrLightVTable;
    AllocatedPbrLight->Data = DataAllocation;
    AllocatedPbrLight->ReferenceCount = 1;

    if (DataSizeInBytes != 0)
    {
        memcpy(DataAllocation, Data, DataSizeInBytes);
    }

    *PbrLight = AllocatedPbrLight;

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PbrLightSample(
    _In_ PCPBR_LIGHT PbrLight,
    _In_ POINT3 HitPoint,
    _Inout_ PPBR_VISIBILITY_TESTER VisibilityTester,
    _Inout_ PRANDOM_REFERENCE Rng,
    _Inout_ PSPECTRUM_COMPOSITOR_REFERENCE Compositor,
    _Out_ PCSPECTRUM *Spectrum,
    _Out_ PVECTOR3 ToPbrLight,
    _Out_ PFLOAT Pdf
    )
{
    ISTATUS Status;

    if (PbrLight == NULL)
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

    if (ToPbrLight == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_06;
    }

    if (Pdf == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_07;
    }

    Status = PbrLight->VTable->SampleRoutine(PbrLight->Data,
                                             HitPoint,
                                             VisibilityTester,
                                             Rng,
                                             Compositor, 
                                             Spectrum,
                                             ToPbrLight,
                                             Pdf);

    return Status;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PbrLightComputeEmissive(
    _In_ PCPBR_LIGHT PbrLight,
    _In_ RAY ToPbrLight,
    _Inout_ PPBR_VISIBILITY_TESTER Tester,
    _Out_ PCSPECTRUM *Spectrum
    )
{
    ISTATUS Status;

    if (PbrLight == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (RayValidate(ToPbrLight) == FALSE)
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

    Status = PbrLight->VTable->ComputeEmissiveRoutine(PbrLight->Data,
                                                      ToPbrLight,
                                                      Tester,
                                                      Spectrum);

    return Status;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
PbrLightComputeEmissiveWithPdf(
    _In_ PCPBR_LIGHT PbrLight,
    _In_ RAY ToPbrLight,
    _Inout_ PPBR_VISIBILITY_TESTER Tester,
    _Out_ PCSPECTRUM *Spectrum,
    _Out_ PFLOAT Pdf
    )
{
    ISTATUS Status;

    if (PbrLight == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (RayValidate(ToPbrLight) == FALSE)
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

    Status = PbrLight->VTable->ComputeEmissiveWithPdfRoutine(PbrLight->Data,
                                                             ToPbrLight,
                                                             Tester,
                                                             Spectrum,
                                                             Pdf);

    return Status;
}

VOID
PbrLightRetain(
    _In_opt_ PPBR_LIGHT PbrLight
    )
{
    if (PbrLight == NULL)
    {
        return;
    }

    PbrLight->ReferenceCount += 1;
}

VOID
PbrLightRelease(
    _In_opt_ _Post_invalid_ PPBR_LIGHT PbrLight
    )
{
    PFREE_ROUTINE FreeRoutine;

    if (PbrLight == NULL)
    {
        return;
    }

    PbrLight->ReferenceCount -= 1;

    if (PbrLight->ReferenceCount == 0)
    {
        FreeRoutine = PbrLight->VTable->FreeRoutine;

        if (FreeRoutine != NULL)
        {
            FreeRoutine(PbrLight->Data);
        }

        IrisAlignedFree(PbrLight);
    }
}