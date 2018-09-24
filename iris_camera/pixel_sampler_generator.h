/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    pixel_sampler_generator.h

Abstract:

    Generates a pixel sampler generator.

--*/

#ifndef _IRIS_CAMERA_PIXEL_SAMPLER_GENERATOR_
#define _IRIS_CAMERA_PIXEL_SAMPLER_GENERATOR_

#include "iris_camera/pixel_sampler.h"

//
// Types
//

ISTATUS
(*PPIXEL_SAMPLER_GENERATE_ROUTINE)(
    _In_ const void *context,
    _Out_ PIXEL_SAMPLER *pixel_sampler
    );

typedef struct _PIXEL_SAMPLER_GENERATOR PIXEL_SAMPLER_GENERATOR;
typedef struct PIXEL_SAMPLER_GENERATOR *PPIXEL_SAMPLER_GENERATOR;
typedef const PIXEL_SAMPLER_GENERATOR *PCPIXEL_SAMPLER_GENERATOR;

//
// Functions
//

ISTATUS
PixelSamplerGeneratorAllocate(
    _In_ PPIXEL_SAMPLER_GENERATE_ROUTINE generate_routine,
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