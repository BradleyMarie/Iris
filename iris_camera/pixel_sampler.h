/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    pixel_sampler.h

Abstract:

    Samples the UV space on the lens and framebuffer.

--*/

#ifndef _IRIS_CAMERA_PIXEL_SAMPLER_
#define _IRIS_CAMERA_PIXEL_SAMPLER_

#include "iris_camera/pixel_sampler_vtable.h"

//
// Types
//

typedef struct _PIXEL_SAMPLER PIXEL_SAMPLER, *PPIXEL_SAMPLER;
typedef const PIXEL_SAMPLER *PCPIXEL_SAMPLER;

//
// Functions
//

ISTATUS
PixelSamplerAllocate(
    _In_ PCPIXEL_SAMPLER_VTABLE vtable,
    _In_reads_bytes_opt_(data_size) const void *data,
    _In_ size_t data_size,
    _In_ size_t data_alignment,
    _Out_ PPIXEL_SAMPLER *pixel_sampler
    );

void
PixelSamplerFree(
    _In_opt_ _Post_invalid_ PPIXEL_SAMPLER pixel_sampler
    );

#endif // _IRIS_CAMERA_PIXEL_SAMPLER_