/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisspectrum_SpectrumCompositorReferencereference.h

Abstract:

    This file contains the definitions for the SPECTRUM_COMPOSITOR_REFERENCE type.

--*/

#ifndef _SPECTRUM_COMPOSITOR_REFERENCE_IRIS_SPECTRUM_
#define _SPECTRUM_COMPOSITOR_REFERENCE_IRIS_SPECTRUM_

#include <irisspectrum.h>

//
// Types
//

typedef struct _SPECTRUM_COMPOSITOR_REFERENCE SPECTRUM_COMPOSITOR_REFERENCE, *PSPECTRUM_COMPOSITOR_REFERENCE;
typedef CONST SPECTRUM_COMPOSITOR_REFERENCE *PCSPECTRUM_REFERENCE_COMPOSITOR_REFERENCE;

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISSPECTRUMAPI
ISTATUS
SpectrumCompositorReferenceAddSpectrums(
    _Inout_ PSPECTRUM_COMPOSITOR_REFERENCE Compositor,
    _In_opt_ PCSPECTRUM_REFERENCE Spectrum0,
    _In_opt_ PCSPECTRUM_REFERENCE Spectrum1,
    _Out_ PCSPECTRUM_REFERENCE *Sum
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISSPECTRUMAPI
ISTATUS
SpectrumCompositorReferenceAttenuateSpectrum(
    _Inout_ PSPECTRUM_COMPOSITOR_REFERENCE Compositor,
    _In_opt_ PCSPECTRUM_REFERENCE Spectrum,
    _In_ FLOAT Attenuation,
    _Out_ PCSPECTRUM_REFERENCE *AttenuatedSpectrum
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISSPECTRUMAPI
ISTATUS
SpectrumCompositorReferenceAttenuatedAddSpectrums(
    _Inout_ PSPECTRUM_COMPOSITOR_REFERENCE Compositor,
    _In_opt_ PCSPECTRUM_REFERENCE Spectrum0,
    _In_opt_ PCSPECTRUM_REFERENCE Spectrum1,
    _In_ FLOAT Attenuation,
    _Out_ PCSPECTRUM_REFERENCE *AttenuatedSum
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISSPECTRUMAPI
ISTATUS
SpectrumCompositorReferenceAddReflection(
    _Inout_ PSPECTRUM_COMPOSITOR_REFERENCE Compositor,
    _In_opt_ PCSPECTRUM_REFERENCE Spectrum,
    _In_opt_ PCREFLECTOR_REFERENCE Reflector,
    _Out_ PCSPECTRUM_REFERENCE *ReflectedSpectrum
    );
    
_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISSPECTRUMAPI
ISTATUS
SpectrumCompositorReferenceAttenuatedAddReflection(
    _Inout_ PSPECTRUM_COMPOSITOR_REFERENCE Compositor,
    _In_opt_ PCSPECTRUM_REFERENCE Spectrum,
    _In_opt_ PCREFLECTOR_REFERENCE Reflector,
    _In_ FLOAT Attenuation,
    _Out_ PCSPECTRUM_REFERENCE *ReflectedSpectrum
    );

#endif // _SPECTRUM_COMPOSITOR_REFERENCE_IRIS_SPECTRUM_