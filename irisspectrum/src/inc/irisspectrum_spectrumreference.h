/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    irisspectrum_spectrumreference.h

Abstract:

    This file contains the internal definitions for the SPECTRUM_REFERENCE type.

--*/

#ifndef _SPECTRUM_REFERENCE_IRIS_SPECTRUM_INTERNAL_
#define _SPECTRUM_REFERENCE_IRIS_SPECTRUM_INTERNAL_

#include <irisspectrump.h>

//
// Macros
//

#ifdef _IRIS_SPECTRUM_EXPORT_SPECTRUM_REFERENCE_ROUTINES_
#define SpectrumReferenceSample(Spectrum,Wavelength,Intensity) \
        StaticSpectrumReferenceSample(Spectrum,Wavelength,Intensity)
#endif

//
// Types
//

struct _SPECTRUM_REFERENCE {
    PCSPECTRUM_VTABLE VTable;
    PVOID Data;
};

//
// Functions
//

SFORCEINLINE
VOID
SpectrumReferenceInitialize(
    _In_ PCSPECTRUM_VTABLE VTable,
    _In_opt_ PVOID Data,
    _Out_ PSPECTRUM_REFERENCE Spectrum
    )
{
    ASSERT(VTable != NULL);
    ASSERT(Spectrum != NULL);
    
    Spectrum->VTable = VTable;
    Spectrum->Data = Data;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
SpectrumReferenceSample(
    _In_opt_ PCSPECTRUM_REFERENCE Spectrum,
    _In_ FLOAT Wavelength,
    _Out_ PFLOAT Intensity
    )
{
    ISTATUS Status;

    if (IsFiniteFloat(Wavelength) == FALSE ||
        IsGreaterThanOrEqualToZeroFloat(Wavelength) == FALSE)
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

SFORCEINLINE
VOID
SpectrumReferenceDestroy(
    _In_ _Post_invalid_ PSPECTRUM_REFERENCE Spectrum
    )
{
    PFREE_ROUTINE FreeRoutine;

    ASSERT(Spectrum != NULL);

    FreeRoutine = Spectrum->VTable->FreeRoutine;

    if (FreeRoutine != NULL)
    {
        FreeRoutine(Spectrum->Data);
    }
}

#ifdef _IRIS_SPECTRUM_EXPORT_SPECTRUM_REFERENCE_ROUTINES_
#undef SpectrumReferenceSample
#endif

#endif // _SPECTRUM_REFERENCE_IRIS_SPECTRUM_INTERNAL_