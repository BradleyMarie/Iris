/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    interpolated_spectrum.h

Abstract:

    Creates a spectrum which linearly interpolates between samples from a
    spectrum at runtime.

    Wavelengths below the minimum or above the maximum wavelength will be 
    clipped to the closest available sample.

--*/

#ifndef _IRIS_PHYSX_TOOLKIT_INTERPOLATED_SPECTRUM_
#define _IRIS_PHYSX_TOOLKIT_INTERPOLATED_SPECTRUM_

#include "iris_physx/iris_physx.h"

#if __cplusplus 
extern "C" {
#endif // __cplusplus

//
// Functions
//

ISTATUS
InterpolatedSpectrumAllocate(
    _In_reads_(num_samples) const float_t *wavelengths,
    _In_reads_(num_samples) const float_t *intensities,
    _In_ size_t num_samples,
    _Out_ PSPECTRUM *spectrum
    );

ISTATUS
InterpolatedReflectorAllocate(
    _In_reads_(num_samples) const float_t *wavelengths,
    _In_reads_(num_samples) const float_t *reflectances,
    _In_ size_t num_samples,
    _Out_ PREFLECTOR *reflector
    );

#if __cplusplus 
}
#endif // __cplusplus

#endif // _IRIS_PHYSX_TOOLKIT_INTERPOLATED_SPECTRUM_