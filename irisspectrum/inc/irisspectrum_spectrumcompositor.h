/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisspectrum_spectrumcompositor.h

Abstract:

    This file contains the definitions for the SPECTRUM_COMPOSITOR type.

--*/

#ifndef _SPECTRUM_COMPOSITOR_IRIS_SPECTRUM_
#define _SPECTRUM_COMPOSITOR_IRIS_SPECTRUM_

#include <irisspectrum.h>

//
// Types
//

typedef struct _SPECTRUM_COMPOSITOR SPECTRUM_COMPOSITOR, *PSPECTRUM_COMPOSITOR;
typedef CONST SPECTRUM_COMPOSITOR *PCSPECTRUM_COMPOSITOR;

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISSPECTRUMAPI
ISTATUS
SpectrumCompositorAddSpectrums(
    _Inout_ PSPECTRUM_COMPOSITOR Compositor,
    _In_opt_ PCSPECTRUM Spectrum0,
    _In_opt_ PCSPECTRUM Spectrum1,
    _Out_ PCSPECTRUM *Sum
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISSPECTRUMAPI
ISTATUS
SpectrumCompositorAttenuateSpectrum(
    _Inout_ PSPECTRUM_COMPOSITOR Compositor,
    _In_opt_ PCSPECTRUM Spectrum,
    _In_ FLOAT Attenuation,
    _Out_ PCSPECTRUM *AttenuatedSpectrum
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISSPECTRUMAPI
ISTATUS
SpectrumCompositorAttenuatedAddSpectrums(
    _Inout_ PSPECTRUM_COMPOSITOR Compositor,
    _In_opt_ PCSPECTRUM Spectrum0,
    _In_opt_ PCSPECTRUM Spectrum1,
    _In_ FLOAT Attenuation,
    _Out_ PCSPECTRUM *AttenuatedSum
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISSPECTRUMAPI
ISTATUS
SpectrumCompositorAddReflection(
    _Inout_ PSPECTRUM_COMPOSITOR Compositor,
    _In_opt_ PCSPECTRUM Spectrum,
    _In_opt_ PCREFLECTOR Reflector,
    _Out_ PCSPECTRUM *ReflectedSpectrum
    );

_Check_return_
_Ret_maybenull_
IRISSPECTRUMAPI
PSPECTRUM_COMPOSITOR
SpectrumCompositorAllocate(
    VOID
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISSPECTRUMAPI
ISTATUS
SpectrumCompositorClear(
    _Inout_ PSPECTRUM_COMPOSITOR Compositor
    );

IRISSPECTRUMAPI
VOID
SpectrumCompositorFree(
    _In_opt_ _Post_invalid_ PSPECTRUM_COMPOSITOR Compositor
    );
    
#endif // _SPECTRUM_COMPOSITOR_IRIS_SPECTRUM_