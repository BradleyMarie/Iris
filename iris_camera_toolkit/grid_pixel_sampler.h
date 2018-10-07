/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    grid_pixel_sampler.h

Abstract:

    Creates a pixel sampler which samples the lens and pixel using a grid super
    sampling algorithm with optional jittering.
    
    The number of U and V samples can be controlled independently for both the
    lens and pixel UV space and must be greater than zero. In the event the size
    of the UV space over either the pixel, the lens, or both are infinitesimal
    the number of samples passed here will be ignored and a single sample will
    be used instead over each infinitesimal UV space.

--*/

#ifndef _IRIS_CAMERA_TOOLKIT_GRID_PIXEL_SAMPLER_
#define _IRIS_CAMERA_TOOLKIT_GRID_PIXEL_SAMPLER_

#include "iris_camera/iris_camera.h"

#if __cplusplus 
extern "C" {
#endif // __cplusplus

//
// Functions
//

ISTATUS
GridPixelSamplerAllocate(
    _In_ uint16_t pixel_samples_u,
    _In_ uint16_t pixel_samples_v,
    _In_ bool jitter_pixel_samples,
    _In_ uint16_t lens_samples_u,
    _In_ uint16_t lens_samples_v,
    _In_ bool jitter_lens_samples,
    _Out_ PPIXEL_SAMPLER *pixel_sampler
    );

#if __cplusplus 
}
#endif // __cplusplus

#endif // _IRIS_CAMERA_TOOLKIT_GRID_PIXEL_SAMPLER_