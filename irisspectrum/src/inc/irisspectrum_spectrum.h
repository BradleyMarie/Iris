/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    irisspectrum_spectrum.h

Abstract:

    This file contains the internal definitions for the SPECTRUM_REFERENCE type.

--*/

#ifndef _SPECTRUM_IRIS_SPECTRUM_INTERNAL_
#define _SPECTRUM_IRIS_SPECTRUM_INTERNAL_

#include <irisspectrump.h>

//
// Macros
//

#ifdef _IRIS_SPECTRUM_EXPORT_SPECTRUM_ROUTINES_
#define SpectrumSample(Spectrum,Wavelength,Intensity) \
        StaticSpectrumSample(Spectrum,Wavelength,Intensity)
#endif

//
// Types
//

struct _SPECTRUM {
    PCSPECTRUM_VTABLE VTable;
    SIZE_T ReferenceCount;
    PVOID Data;
};

//
// Functions
//

SFORCEINLINE
VOID
SpectrumInitialize(
    _In_ PCSPECTRUM_VTABLE VTable,
    _In_opt_ PVOID Data,
    _Out_ PSPECTRUM Spectrum
    )
{
    ASSERT(VTable != NULL);
    ASSERT(Spectrum != NULL);
    
    Spectrum->VTable = VTable;
    Spectrum->ReferenceCount = 1;
    Spectrum->Data = Data;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
SpectrumSample(
    _In_opt_ PCSPECTRUM Spectrum,
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

#ifdef _IRIS_SPECTRUM_EXPORT_SPECTRUM_ROUTINES_
#undef SpectrumSample
#endif

#endif // _SPECTRUM_IRIS_SPECTRUM_INTERNAL_