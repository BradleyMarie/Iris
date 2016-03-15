/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisspectrum_spectrum.h

Abstract:

    This file contains the definitions for the SPECTRUM type.

--*/

#ifndef _SPECTRUM_IRIS_SPECTRUM_
#define _SPECTRUM_IRIS_SPECTRUM_

#include <irisspectrum.h>

//
// Types
//

typedef
_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
(*PSPECTRUM_SAMPLE_ROUTINE)(
    _In_ PCVOID Context,
    _In_ FLOAT Wavelength,
    _Out_ PFLOAT Intensity
    );

typedef struct _SPECTRUM_VTABLE {
    PSPECTRUM_SAMPLE_ROUTINE SampleRoutine;
    PFREE_ROUTINE FreeRoutine;
} SPECTRUM_VTABLE, *PSPECTRUM_VTABLE;

typedef CONST SPECTRUM_VTABLE *PCSPECTRUM_VTABLE;

typedef struct _SPECTRUM SPECTRUM, *PSPECTRUM;
typedef CONST SPECTRUM *PCSPECTRUM;

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISSPECTRUMAPI
ISTATUS
SpectrumAllocate(
    _In_ PCSPECTRUM_VTABLE SpectrumVTable,
    _When_(DataSizeInBytes != 0, _In_reads_bytes_opt_(DataSizeInBytes)) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _When_(DataSizeInBytes != 0, _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && DataSizeInBytes % _Curr_ == 0)) SIZE_T DataAlignment,
    _Out_ PSPECTRUM *Spectrum
    );

#ifndef _DISABLE_IRIS_SPECTRUM_SPECTRUM_EXPORTS_

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISSPECTRUMAPI
ISTATUS
SpectrumSample(
    _In_opt_ PCSPECTRUM Spectrum,
    _In_ FLOAT Wavelength,
    _Out_ PFLOAT Intensity
    );

#endif // _DISABLE_IRIS_SPECTRUM_SPECTRUM_EXPORTS_

IRISSPECTRUMAPI
VOID
SpectrumRetain(
    _In_opt_ PSPECTRUM Spectrum
    );

IRISSPECTRUMAPI
VOID
SpectrumRelease(
    _In_opt_ _Post_invalid_ PSPECTRUM Spectrum
    );

#endif // _SPECTRUM_IRIS_SPECTRUM_