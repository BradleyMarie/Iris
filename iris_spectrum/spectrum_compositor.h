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

ISTATUS
SpectrumCompositorAddSpectra(
    _Inout_ PSPECTRUM_COMPOSITOR compositor,
    _In_opt_ PCSPECTRUM spectrum0,
    _In_opt_ PCSPECTRUM spectrum1,
    _Out_ PCSPECTRUM *sum_spectrum
    );

ISTATUS
SpectrumCompositorAttenuateSpectrum(
    _Inout_ PSPECTRUM_COMPOSITOR compositor,
    _In_opt_ PCSPECTRUM spectrum,
    _In_ float_t attenuation,
    _Out_ PCSPECTRUM *attenuated_spectrum
    );

ISTATUS
SpectrumCompositorAddReflection(
    _Inout_ PSPECTRUM_COMPOSITOR compositor,
    _In_opt_ PCSPECTRUM spectrum,
    _In_opt_ PCREFLECTOR reflector,
    _Out_ PCSPECTRUM *reflected_spectrum
    );

ISTATUS
SpectrumCompositorAttenuatedAddReflection(
    _Inout_ PSPECTRUM_COMPOSITOR compositor,
    _In_opt_ PCSPECTRUM spectrum,
    _In_opt_ PCREFLECTOR reflector,
    _In_ float_t attenuation,
    _Out_ PCSPECTRUM *reflected_spectrum
    );
    
#endif // _IRIS_SPECTRUM_SPECTRUM_COMPOSITOR_