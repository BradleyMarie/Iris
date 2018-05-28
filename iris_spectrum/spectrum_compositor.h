/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    spectrum_compositor.h

Abstract:

    Enables the composition of spectra and reflectors into new spectra.

    The lifetimes of any spectra allocated by a spectrum compositor match the
    lifetime of the compositor and will be freed automatically when the 
    compositor goes out of scope.

--*/

#ifndef _IRIS_SPECTRUM_SPECTRUM_COMPOSITOR_
#define _IRIS_SPECTRUM_SPECTRUM_COMPOSITOR_

#include "iris_spectrum/reflector.h"
#include "iris_spectrum/spectrum.h"

//
// Types
//

typedef struct _SPECTRUM_COMPOSITOR SPECTRUM_COMPOSITOR, *PSPECTRUM_COMPOSITOR;
typedef const SPECTRUM_COMPOSITOR *PCSPECTRUM_COMPOSITOR;

//
// Functions
//

//IRISSPECTRUMAPI
ISTATUS
SpectrumCompositorAddSpectra(
    _Inout_ PSPECTRUM_COMPOSITOR Compositor,
    _In_opt_ PCSPECTRUM Spectrum0,
    _In_opt_ PCSPECTRUM Spectrum1,
    _Out_ PCSPECTRUM *Sum
    );

//IRISSPECTRUMAPI
ISTATUS
SpectrumCompositorAttenuateSpectrum(
    _Inout_ PSPECTRUM_COMPOSITOR Compositor,
    _In_opt_ PCSPECTRUM Spectrum,
    _In_ float_t Attenuation,
    _Out_ PCSPECTRUM *AttenuatedSpectrum
    );

//IRISSPECTRUMAPI
ISTATUS
SpectrumCompositorAddReflection(
    _Inout_ PSPECTRUM_COMPOSITOR Compositor,
    _In_opt_ PCSPECTRUM Spectrum,
    _In_opt_ PCREFLECTOR Reflector,
    _Out_ PCSPECTRUM *ReflectedSpectrum
    );

//IRISSPECTRUMAPI
ISTATUS
SpectrumCompositorAttenuatedAddReflection(
    _Inout_ PSPECTRUM_COMPOSITOR Compositor,
    _In_opt_ PCSPECTRUM Spectrum,
    _In_opt_ PCREFLECTOR Reflector,
    _In_ float_t Attenuation,
    _Out_ PCSPECTRUM *ReflectedSpectrum
    );
    
#endif // _IRIS_SPECTRUM_SPECTRUM_COMPOSITOR_