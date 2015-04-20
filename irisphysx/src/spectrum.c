/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    spectrum.c

Abstract:

    This file contains the definitions for the SPECTRUM type.

--*/

#include <irisphysxp.h>

//
// Functions
//

_Check_return_
_Ret_maybenull_
PSPECTRUM
SpectrumAllocate(
    _In_ PCSPECTRUM_VTABLE SpectrumVTable,
    _In_reads_bytes_(DataSizeInBytes) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _In_ SIZE_T DataAlignment
    )
{
    PVOID HeaderAllocation;
    PVOID DataAllocation;
    PSPECTRUM Spectrum;

    if (SpectrumVTable == NULL)
    {
        return NULL;
    }

    if (Data == NULL && DataSizeInBytes == 0)
    {
        return NULL;
    }

    HeaderAllocation = IrisAlignedMallocWithHeader(sizeof(SPECTRUM),
                                                   sizeof(PVOID),
                                                   DataSizeInBytes,
                                                   DataAlignment,
                                                   &DataAllocation);

    if (HeaderAllocation == NULL)
    {
        return NULL;
    }

    Spectrum = (PSPECTRUM) HeaderAllocation;

    Spectrum->VTable = SpectrumVTable;
    Spectrum->Data = DataAllocation;
    Spectrum->ReferenceCount = 1;

    if (DataSizeInBytes != 0)
    {
        memcpy(DataAllocation, Data, DataSizeInBytes);
    }

    return Spectrum;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
SpectrumSample(
    _In_ PCSPECTRUM Spectrum,
    _In_ FLOAT Wavelength,
    _Out_ PFLOAT Intensity
    )
{
    ISTATUS Status;

    if (Spectrum == NULL ||
        IsNormalFloat(Wavelength) == FALSE ||
        IsFiniteFloat(Wavelength) == FALSE ||
        IsPositiveFloat(Wavelength) == FALSE ||
        Intensity == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT;
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