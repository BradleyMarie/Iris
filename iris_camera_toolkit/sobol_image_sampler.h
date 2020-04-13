/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    sobol_image_sampler.h

Abstract:

    Creates an image sampler which samples the lens and pixel using a sobol
    sequence based super sampling pattern.

--*/

#ifndef _IRIS_CAMERA_TOOLKIT_SOBOL_IMAGE_SAMPLER_
#define _IRIS_CAMERA_TOOLKIT_SOBOL_IMAGE_SAMPLER_

#include "iris_camera/iris_camera.h"

#if __cplusplus 
extern "C" {
#endif // __cplusplus

//
// Functions
//

ISTATUS
SobolImageSamplerAllocate(
    _In_ uint16_t samples_per_pixel,
    _Out_ PIMAGE_SAMPLER *image_sampler
    );

#if __cplusplus 
}
#endif // __cplusplus

#endif // _IRIS_CAMERA_TOOLKIT_SOBOL_IMAGE_SAMPLER_