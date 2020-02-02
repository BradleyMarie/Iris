/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    rgb_spectra.h

Abstract:

    A spectrum, reflector, color integrator, and color extrapolator for use in
    non-spectral rendering which only use the r, g, and b values of the color
    of a reflector or spectrum in shading computations.

--*/

#ifndef _IRIS_PHYSX_TOOLKIT_RGB_SPECTRA_
#define _IRIS_PHYSX_TOOLKIT_RGB_SPECTRA_

#include "iris_physx/iris_physx.h"
#include "iris_physx_toolkit/color_extrapolator.h"

#if __cplusplus 
extern "C" {
#endif // __cplusplus

//
// Functions
//

ISTATUS
RgbSpectrumAllocate(
    _In_ float_t r,
    _In_ float_t g,
    _In_ float_t b,
    _Out_ PSPECTRUM *spectrum
    );

ISTATUS
RgbReflectorAllocate(
    _In_ float_t r,
    _In_ float_t g,
    _In_ float_t b,
    _Out_ PREFLECTOR *reflector
    );

ISTATUS
RgbColorIntegratorAllocate(
    _Out_ PCOLOR_INTEGRATOR *color_integrator
    );

ISTATUS
RgbColorExtrapolatorAllocate(
    _Out_ PCOLOR_EXTRAPOLATOR *color_extrapolator
    );

#if __cplusplus 
}
#endif // __cplusplus

#endif // _IRIS_PHYSX_TOOLKIT_RGB_SPECTRA_