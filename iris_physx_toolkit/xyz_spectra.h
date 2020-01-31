/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    xyz_spectra.h

Abstract:

    A spectrum, reflector, color integrator, and color extrapolator for use in
    non-spectral rendering which only use the x, y, and z values of the color
    of a reflector or spectrum in shading computations.

--*/

#ifndef _IRIS_PHYSX_TOOLKIT_XYZ_SPECTRA_
#define _IRIS_PHYSX_TOOLKIT_XYZ_SPECTRA_

#include "iris_physx/iris_physx.h"
#include "iris_physx_toolkit/color_extrapolator.h"

#if __cplusplus 
extern "C" {
#endif // __cplusplus

//
// Functions
//

ISTATUS
XyzSpectrumAllocate(
    _In_ float_t x,
    _In_ float_t y,
    _In_ float_t z,
    _Out_ PSPECTRUM *spectrum
    );

ISTATUS
XyzReflectorAllocate(
    _In_ float_t x,
    _In_ float_t y,
    _In_ float_t z,
    _Out_ PREFLECTOR *reflector
    );

ISTATUS
XyzColorIntegratorAllocate(
    _Out_ PCOLOR_INTEGRATOR *color_integrator
    );

ISTATUS
XyzColorExtrapolatorAllocate(
    _Out_ PCOLOR_EXTRAPOLATOR *color_extrapolator
    );

#if __cplusplus 
}
#endif // __cplusplus

#endif // _IRIS_PHYSX_TOOLKIT_XYZ_SPECTRA_