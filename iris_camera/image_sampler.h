/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    image_sampler.h

Abstract:

    Samples the UV space of the lens and framebuffer.

--*/

#ifndef _IRIS_CAMERA_IMAGE_SAMPLER_
#define _IRIS_CAMERA_IMAGE_SAMPLER_

#include "iris_camera/image_sampler_vtable.h"

//
// Functions
//

ISTATUS
ImageSamplerAllocate(
    _In_ PCIMAGE_SAMPLER_VTABLE vtable,
    _In_reads_bytes_opt_(data_size) const void *data,
    _In_ size_t data_size,
    _In_ size_t data_alignment,
    _Out_ PIMAGE_SAMPLER *image_sampler
    );

void
ImageSamplerFree(
    _In_opt_ _Post_invalid_ PIMAGE_SAMPLER image_sampler
    );

#endif // _IRIS_CAMERA_IMAGE_SAMPLER_