/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    pixel_sampler.h

Abstract:

    Samples the UV space on the lens and framebuffer.

--*/

#ifndef _IRIS_CAMERA_PIXEL_SAMPLER_
#define _IRIS_CAMERA_PIXEL_SAMPLER_

#include "iris_advanced/iris_advanced.h"

//
// Types
//

ISTATUS
(*PPIXEL_SAMPLER_GENERATE_SAMPLE_ROUTINE)(
    _In_ const void *context,
    _In_ PRANDOM rng,
    _In_ float_t pixel_sample_min_u,
    _In_ float_t pixel_sample_max_u,
    _In_ float_t pixel_sample_min_v,
    _In_ float_t pixel_sample_max_v,
    _In_ float_t lens_sample_min_u,
    _In_ float_t lens_sample_max_u,
    _In_ float_t lens_sample_min_v,
    _In_ float_t lens_sample_max_v,
    _In_ size_t num_samples,
    _Out_writes_(num_samples) float_t pixel_samples_u[],
    _Out_writes_(num_samples) float_t pixel_samples_v[],
    _Out_writes_(num_samples) float_t lens_samples_u[],
    _Out_writes_(num_samples) float_t lens_samples_v[]
    );

typedef struct _PIXEL_SAMPLER PIXEL_SAMPLER, *PPIXEL_SAMPLER;
typedef const PIXEL_SAMPLER *PCPIXEL_SAMPLER;

//
// Functions
//

ISTATUS
PixelSamplerAllocate(
    _In_ PPIXEL_SAMPLER_GENERATE_SAMPLE_ROUTINE generate_sample_routine,
    _In_ size_t samples_per_pixel,
    _In_reads_bytes_opt_(data_size) const void *data,
    _In_ size_t data_size,
    _In_ size_t data_alignment,
    _Out_ PPIXEL_SAMPLER pixel_sampler
    );

void
PixelSamplerFree(
    _In_opt_ _Post_invalid_ PPIXEL_SAMPLER pixel_sampler
    );

#endif // _IRIS_CAMERA_PIXEL_SAMPLER_