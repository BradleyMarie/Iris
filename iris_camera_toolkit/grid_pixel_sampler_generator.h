/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    grid_pixel_sampler_generator.h

Abstract:

    Creates a generator for grid pixel samplers.

--*/

#ifndef _IRIS_CAMERA_TOOLKIT_GRID_PIXEL_SAMPLER_GENERATOR_
#define _IRIS_CAMERA_TOOLKIT_GRID_PIXEL_SAMPLER_GENERATOR_

#include "iris_camera/iris_camera.h"

#if __cplusplus 
extern "C" {
#endif // __cplusplus

//
// Functions
//

ISTATUS
GridPixelSamplerGeneratorAllocate(
    _In_ uint16_t pixel_samples_u,
    _In_ uint16_t pixel_samples_v,
    _In_ bool jitter_pixel_samples,
    _In_ uint16_t lens_samples_u,
    _In_ uint16_t lens_samples_v,
    _In_ bool jitter_lens_samples,
    _Out_ PPIXEL_SAMPLER_GENERATOR *pixel_sampler_generator
    );

#if __cplusplus 
}
#endif // __cplusplus

#endif // _IRIS_CAMERA_TOOLKIT_GRID_PIXEL_SAMPLER_GENERATOR_