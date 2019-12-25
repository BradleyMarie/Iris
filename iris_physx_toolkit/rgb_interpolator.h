/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    rgb_interpolate.h

Abstract:

    Generates an interpolated spectrum or reflector from an RGB color. Spectrum
    will be interpolated using samples at the wavelengths passed at creation.

--*/

#ifndef _IRIS_PHYSX_TOOLKIT_RGB_INTERPOLATOR_
#define _IRIS_PHYSX_TOOLKIT_RGB_INTERPOLATOR_

#include "iris_physx/iris_physx.h"

#if __cplusplus 
extern "C" {
#endif // __cplusplus

//
// Defines
//

typedef struct _RGB_INTERPOLATOR RGB_INTERPOLATOR, *PRGB_INTERPOLATOR;
typedef const RGB_INTERPOLATOR *PCRGB_INTERPOLATOR;

//
// Functions
//

ISTATUS
RgbInterpolatorAllocate(
    _In_reads_(num_wavelengths) float_t wavelengths[],
    _In_ size_t num_wavelengths,
    _Out_ PRGB_INTERPOLATOR* interpolator
    );

ISTATUS
RgbInterpolatorAllocateSpectrum(
    _In_ PCRGB_INTERPOLATOR interpolator,
    _In_ float_t r,
    _In_ float_t g,
    _In_ float_t b,
    _Out_ PSPECTRUM *spectrum
    );

ISTATUS
RgbInterpolatorAllocateReflector(
    _In_ PCRGB_INTERPOLATOR interpolator,
    _In_ float_t r,
    _In_ float_t g,
    _In_ float_t b,
    _Out_ PREFLECTOR *reflector
    );

void
RgbInterpolatorFree(
    _In_opt_ _Post_invalid_ PRGB_INTERPOLATOR interpolator
    );

#if __cplusplus 
}
#endif // __cplusplus

#endif // _IRIS_PHYSX_TOOLKIT_RGB_INTERPOLATOR_