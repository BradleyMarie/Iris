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
_Ret_maybenull_
IRISSPECTRUMAPI
PSPECTRUM
SpectrumAllocate(
    _In_ PCSPECTRUM_VTABLE SpectrumVTable,
    _In_reads_bytes_(DataSizeInBytes) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _In_ SIZE_T DataAlignment
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISSPECTRUMAPI
ISTATUS
SpectrumSample(
    _In_opt_ PCSPECTRUM Spectrum,
    _In_ FLOAT Wavelength,
    _Out_ PFLOAT Intensity
    );

IRISSPECTRUMAPI
VOID
SpectrumReference(
    _In_opt_ PSPECTRUM Spectrum
    );

IRISSPECTRUMAPI
VOID
SpectrumDereference(
    _In_opt_ _Post_invalid_ PSPECTRUM Spectrum
    );

#endif // _SPECTRUM_IRIS_SPECTRUM_