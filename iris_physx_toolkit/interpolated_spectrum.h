/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    interpolated_spectrum.h

Abstract:

    Creates a spectrum which linearly interpolates between samples from a
    spectrum at runtime.

    Wavelengths below the minimum or above the maximum wavelength will be 
    clipped to zero.

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

#if __cplusplus 
}
#endif // __cplusplus

#endif // _IRIS_PHYSX_TOOLKIT_INTERPOLATED_SPECTRUM_