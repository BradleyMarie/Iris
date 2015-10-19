/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    spectrum.c

Abstract:

    This file contains the definitions for the SPECTRUM type.

--*/

#include <irisspectrump.h>

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
SpectrumAllocate(
    _In_ PCSPECTRUM_VTABLE SpectrumVTable,
    _When_(DataSizeInBytes != 0, _In_reads_bytes_opt_(DataSizeInBytes)) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _When_(DataSizeInBytes != 0, _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && DataSizeInBytes % _Curr_ == 0)) SIZE_T DataAlignment,
    _Out_ PSPECTRUM *Spectrum
    )
{
    BOOL AllocationSuccessful;
    PVOID HeaderAllocation;
    PVOID DataAllocation;
    PSPECTRUM AllocatedSpectrum;

    if (SpectrumVTable == NULL)
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

    if (Spectrum == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    AllocationSuccessful = IrisAlignedAllocWithHeader(sizeof(SPECTRUM),
                                                      _Alignof(SPECTRUM),
                                                      &HeaderAllocation,
                                                      DataSizeInBytes,
                                                      DataAlignment,
                                                      &DataAllocation,
                                                      NULL);

    if (AllocationSuccessful == FALSE)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    AllocatedSpectrum = (PSPECTRUM) HeaderAllocation;

    AllocatedSpectrum->VTable = SpectrumVTable;
    AllocatedSpectrum->Data = DataAllocation;
    AllocatedSpectrum->ReferenceCount = 1;

    if (DataSizeInBytes != 0)
    {
        memcpy(DataAllocation, Data, DataSizeInBytes);
    }

    *Spectrum = AllocatedSpectrum;

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
SpectrumSample(
    _In_opt_ PCSPECTRUM Spectrum,
    _In_ FLOAT Wavelength,
    _Out_ PFLOAT Intensity
    )
{
    ISTATUS Status;

    if (IsNormalFloat(Wavelength) == FALSE ||
        IsFiniteFloat(Wavelength) == FALSE ||
        IsPositiveFloat(Wavelength) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }
    
    if (Intensity == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (Spectrum == NULL)
    {
        *Intensity = (FLOAT) 0.0f;
        return ISTATUS_SUCCESS;
    }

    Status = Spectrum->VTable->SampleRoutine(Spectrum->Data,
                                             Wavelength,
                                             Intensity);

    return Status;
}

VOID
SpectrumReference(
    _In_opt_ PSPECTRUM Spectrum
    )
{
    if (Spectrum == NULL)
    {
        return;
    }

    Spectrum->ReferenceCount += 1;
}

VOID
SpectrumDereference(
    _In_opt_ _Post_invalid_ PSPECTRUM Spectrum
    )
{
    PFREE_ROUTINE FreeRoutine;

    if (Spectrum == NULL)
    {
        return;
    }

    Spectrum->ReferenceCount -= 1;

    if (Spectrum->ReferenceCount == 0)
    {
        FreeRoutine = Spectrum->VTable->FreeRoutine;

        if (FreeRoutine != NULL)
        {
            FreeRoutine(Spectrum->Data);
        }

        IrisAlignedFree(Spectrum);
    }
}