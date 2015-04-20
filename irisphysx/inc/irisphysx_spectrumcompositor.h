/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisphysx_spectrumcompositor.h

Abstract:

    This file contains the definitions for the SPECTRUM_COMPOSITOR type.

--*/

#ifndef _SPECTRUM_COMPOSITOR_IRIS_PHYSX_
#define _SPECTRUM_COMPOSITOR_IRIS_PHYSX_

#include <irisphysx.h>

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
IRISPHYSXAPI
ISTATUS
SpectrumCompositorAddSpectrums(
    _Inout_ PSPECTRUM_COMPOSITOR Compositor,
    _In_ PCSPECTRUM Spectrum0,
    _In_ PCSPECTRUM Spectrum1,
    _Out_ PCSPECTRUM *Sum
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
SpectrumCompositorAttenuateSpectrum(
    _Inout_ PSPECTRUM_COMPOSITOR Compositor,
    _In_ PCSPECTRUM Spectrum,
    _In_ FLOAT Attenuation,
    _Out_ PCSPECTRUM *AttenuatedSpectrum
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
SpectrumCompositorAttenuatedAddSpectrums(
    _Inout_ PSPECTRUM_COMPOSITOR Compositor,
    _In_ PCSPECTRUM Spectrum0,
    _In_ PCSPECTRUM Spectrum1,
    _In_ FLOAT Attenuation,
    _Out_ PCSPECTRUM *AttenuatedSum
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
SpectrumCompositorAddReflection(
    _Inout_ PSPECTRUM_COMPOSITOR Compositor,
    _In_ PCSPECTRUM Spectrum,
    _In_ PCREFLECTOR Reflector,
    _Out_ PCSPECTRUM *ReflectedSpectrum
    );

#endif // _SPECTRUM_COMPOSITOR_IRIS_PHYSX_