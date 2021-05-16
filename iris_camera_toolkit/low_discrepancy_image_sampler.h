/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    low_discrepancy_image_sampler.h

Abstract:

    Creates an image sampler which samples using a low discrepancy sequence
    based super sampling pattern.

--*/

#ifndef _IRIS_CAMERA_TOOLKIT_LOW_DISCREPANCY_IMAGE_SAMPLER_
#define _IRIS_CAMERA_TOOLKIT_LOW_DISCREPANCY_IMAGE_SAMPLER_

#include "iris_advanced_toolkit/low_discrepancy_sequence.h"
#include "iris_camera/iris_camera.h"

#if __cplusplus 
extern "C" {
#endif // __cplusplus

//
// Functions
//

ISTATUS
LowDiscrepancyImageSamplerAllocate(
    _In_ PCLOW_DISCREPANCY_SEQUENCE sequence,
    _In_ uint32_t samples_per_pixel,
    _Out_ PIMAGE_SAMPLER *image_sampler
    );

#if __cplusplus 
}
#endif // __cplusplus

#endif // _IRIS_CAMERA_TOOLKIT_LOW_DISCREPANCY_IMAGE_SAMPLER_