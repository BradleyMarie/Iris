/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    color_spectra.h

Abstract:

    A spectrum, reflector, color integrator, and color extrapolator for use in
    non-spectral rendering which only use the color of a reflector or spectrum
    in shading computations.

--*/

#ifndef _IRIS_PHYSX_TOOLKIT_COLOR_SPECTRA_
#define _IRIS_PHYSX_TOOLKIT_COLOR_SPECTRA_

#include "iris_physx/iris_physx.h"
#include "iris_physx_toolkit/color_extrapolator.h"

#if __cplusplus 
extern "C" {
#endif // __cplusplus

//
// Functions
//

ISTATUS
ColorColorIntegratorAllocate(
    _In_ COLOR_SPACE color_space,
    _Out_ PCOLOR_INTEGRATOR *color_integrator
    );

ISTATUS
ColorColorExtrapolatorAllocate(
    _In_ COLOR_SPACE color_space,
    _Out_ PCOLOR_EXTRAPOLATOR *color_extrapolator
    );

#if __cplusplus 
}
#endif // __cplusplus

#endif // _IRIS_PHYSX_TOOLKIT_COLOR_SPECTRA_