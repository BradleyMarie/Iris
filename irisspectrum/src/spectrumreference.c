/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    spectrumreference.c

Abstract:

    This file contains the definitions for the SPECTRUM_REFERENCE type.

--*/

#define _IRIS_SPECTRUM_EXPORT_SPECTRUM_REFERENCE_ROUTINES_
#include <irisspectrump.h>

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
SpectrumReferenceReflect(
    _In_opt_ PCSPECTRUM_REFERENCE Spectrum,
    _In_ FLOAT Wavelength,
    _Out_ PFLOAT Intensity
    )
{
    ISTATUS Status;

    Status = StaticSpectrumReferenceSample(Spectrum,
                                           Wavelength,
                                           Intensity);

    return Status;
}