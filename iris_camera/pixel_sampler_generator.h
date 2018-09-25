/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    pixel_sampler_generator.h

Abstract:

    Generates a pixel sampler generator.

--*/

#ifndef _IRIS_CAMERA_PIXEL_SAMPLER_GENERATOR_
#define _IRIS_CAMERA_PIXEL_SAMPLER_GENERATOR_

#include "iris_camera/pixel_sampler_generator_vtable.h"

//
// Types
//

typedef struct _PIXEL_SAMPLER_GENERATOR PIXEL_SAMPLER_GENERATOR;
typedef PIXEL_SAMPLER_GENERATOR *PPIXEL_SAMPLER_GENERATOR;
typedef const PIXEL_SAMPLER_GENERATOR *PCPIXEL_SAMPLER_GENERATOR;

//
// Functions
//

ISTATUS
PixelSamplerGeneratorAllocate(
    _In_ PCPIXEL_SAMPLER_GENERATOR_VTABLE vtable,
    _In_reads_bytes_opt_(data_size) const void *data,
    _In_ size_t data_size,
    _In_ size_t data_alignment,
    _Out_ PPIXEL_SAMPLER_GENERATOR *pixel_sampler_generator
    );

void
PixelSamplerGeneratorFree(
    _In_opt_ _Post_invalid_ PPIXEL_SAMPLER_GENERATOR pixel_sampler_generator
    );

#endif // _IRIS_CAMERA_PIXEL_SAMPLER_GENERATOR_