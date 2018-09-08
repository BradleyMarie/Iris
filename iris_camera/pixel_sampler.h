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
    _In_ uint32_t num_samples,
    _Out_writes_(num_samples) float_t pixel_samples_u[],
    _Out_writes_(num_samples) float_t pixel_samples_v[],
    _Out_writes_(num_samples) float_t lens_samples_u[],
    _Out_writes_(num_samples) float_t lens_samples_v[]
    );

typedef struct _PIXEL_SAMPLER PIXEL_SAMPLER, *PPIXEL_SAMPLER;
typedef const PIXEL_SAMPLER *PCPIXEL_SAMPLER;

typedef
ISTATUS
(*PPIXEL_SAMPLER_LIFETIME_ROUTINE)(
    _Inout_opt_ void *context,
    _In_ PCPIXEL_SAMPLER sampler
    );

//
// Functions
//

ISTATUS
PixelSamplerCreate(
    _In_ PPIXEL_SAMPLER_GENERATE_SAMPLE_ROUTINE generate_sample_routine,
    _In_ uint32_t samples_per_pixel,
    _In_ const void *sampler_context,
    _In_ PPIXEL_SAMPLER_LIFETIME_ROUTINE callback,
    _In_ void *callback_context
    );

#endif // _IRIS_CAMERA_PIXEL_SAMPLER_