/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    irisspectrum_spectrumreference.h

Abstract:

    This file contains the definitions for the SPECTRUM_REFERENCE type.

--*/

#ifndef _SPECTRUM_REFERENCE_IRIS_SPECTRUM_
#define _SPECTRUM_REFERENCE_IRIS_SPECTRUM_

#include <irisspectrum.h>

//
// Types
//

typedef struct _SPECTRUM_REFERENCE SPECTRUM_REFERENCE, *PSPECTRUM_REFERENCE;
typedef CONST SPECTRUM_REFERENCE *PCSPECTRUM_REFERENCE;

//
// Functions
//

#ifndef _DISABLE_IRIS_SPECTRUM_SPECTRUM_REFERENCE_EXPORTS_

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISSPECTRUMAPI
ISTATUS
SpectrumReferenceSample(
    _In_opt_ PCSPECTRUM_REFERENCE Spectrum,
    _In_ FLOAT Wavelength,
    _Out_ PFLOAT Intensity
    );

#endif //_DISABLE_IRIS_SPECTRUM_SPECTRUM_REFERENCE_EXPORTS_

#endif // _SPECTRUM_REFERENCE_IRIS_SPECTRUM_