/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    smits_color_extrapolator.h

Abstract:

    Generates an interpolated spectrum or reflector from an RGB color. Spectrum
    will be extrapolated using samples at the wavelengths passed at creation.

--*/

#ifndef _IRIS_PHYSX_TOOLKIT_SMITS_COLOR_EXTRAPOLATOR_
#define _IRIS_PHYSX_TOOLKIT_SMITS_COLOR_EXTRAPOLATOR_

#include "iris_physx/iris_physx.h"
#include "iris_physx_toolkit/color_extrapolator.h"

#if __cplusplus 
extern "C" {
#endif // __cplusplus

//
// Functions
//

ISTATUS
SmitsColorExtrapolatorAllocate(
    _In_reads_(num_wavelengths) float_t wavelengths[],
    _In_ size_t num_wavelengths,
    _Out_ PCOLOR_EXTRAPOLATOR* extrapolator
    );

#if __cplusplus 
}
#endif // __cplusplus

#endif // _IRIS_PHYSX_TOOLKIT_SMITS_COLOR_EXTRAPOLATOR_