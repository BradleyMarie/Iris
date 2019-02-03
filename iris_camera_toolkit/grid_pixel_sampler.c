/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    grid_pixel_sampler.c

Abstract:

    Implements a grid pixel sampler.

--*/

#include <stdalign.h>

#include "common/safe_math.h"
#include "iris_camera_toolkit/grid_pixel_sampler.h"

//
// Types
//

typedef struct _GRID_PIXEL_SAMPLER {
    uint16_t pixel_samples_u;
    uint16_t pixel_samples_v;
    uint16_t lens_samples_u;
    uint16_t lens_samples_v;
    float_t pixel_min_u;
    float_t pixel_min_v;
    float_t lens_min_u;
    float_t lens_min_v;
    float_t pixel_max_u;
    float_t pixel_max_v;
    float_t lens_max_u;
    float_t lens_max_v;
    float_t pixel_sample_width_u;
    float_t pixel_sample_width_v;
    float_t lens_sample_width_u;
    float_t lens_sample_width_v;
    bool jitter_pixel_samples;
    bool jitter_lens_samples;
    uint16_t current_pixel_samples_u;
    uint16_t current_pixel_samples_v;
    uint16_t current_lens_samples_u;
    uint16_t current_lens_samples_v;
} GRID_PIXEL_SAMPLER, *PGRID_PIXEL_SAMPLER;

typedef const GRID_PIXEL_SAMPLER *PCGRID_PIXEL_SAMPLER;

//
// Static Functions
//

ISTATUS
GridPixelSamplerPrepareSamples(
    _In_ void *context,
    _Inout_ PRANDOM rng,
    _In_ float_t pixel_min_u,
    _In_ float_t pixel_max_u,
    _In_ float_t pixel_min_v,
    _In_ float_t pixel_max_v,
    _In_ float_t lens_min_u,
    _In_ float_t lens_max_u,
    _In_ float_t lens_min_v,
    _In_ float_t lens_max_v,
    _Out_ size_t *num_samples
    )
{
    PGRID_PIXEL_SAMPLER pixel_sampler = (PGRID_PIXEL_SAMPLER)context;

    if (pixel_min_u == pixel_max_u)
    {
        pixel_sampler->current_pixel_samples_u = 1;
    }
    else
    {
        pixel_sampler->current_pixel_samples_u =
            pixel_sampler->pixel_samples_u;
    }

    if (pixel_min_v == pixel_max_v)
    {
        pixel_sampler->current_pixel_samples_v = 1;
    }
    else
    {
        pixel_sampler->current_pixel_samples_v =
            pixel_sampler->pixel_samples_v;
    }

    if (lens_min_u == lens_max_u)
    {
        pixel_sampler->current_lens_samples_u = 1;
    }
    else
    {
        pixel_sampler->current_lens_samples_u =
            pixel_sampler->lens_samples_u;
    }

    if (lens_min_v == lens_max_v)
    {
        pixel_sampler->current_lens_samples_v = 1;
    }
    else
    {
        pixel_sampler->current_lens_samples_v =
            pixel_sampler->lens_samples_v;
    }

    pixel_sampler->pixel_min_u = pixel_min_u;
    pixel_sampler->pixel_min_v = pixel_min_v;
    pixel_sampler->lens_min_u = lens_min_u;
    pixel_sampler->lens_min_v = lens_min_v;
    pixel_sampler->pixel_max_u = pixel_max_u;
    pixel_sampler->pixel_max_v = pixel_max_v;
    pixel_sampler->lens_max_u = lens_max_u;
    pixel_sampler->lens_max_v = lens_max_v;

    pixel_sampler->pixel_sample_width_u =
        (pixel_max_u - pixel_min_u) / (float_t)pixel_sampler->pixel_samples_u;
    pixel_sampler->pixel_sample_width_v =
        (pixel_max_v - pixel_min_v) / (float_t)pixel_sampler->pixel_samples_v;
    pixel_sampler->lens_sample_width_u =
        (lens_max_u - lens_min_u) / (float_t)pixel_sampler->lens_samples_u;
    pixel_sampler->lens_sample_width_v =
        (lens_max_v - lens_min_v) / (float_t)pixel_sampler->lens_samples_v;

    *num_samples = pixel_sampler->current_pixel_samples_u *
                   pixel_sampler->current_pixel_samples_v *
                   pixel_sampler->current_lens_samples_u *
                   pixel_sampler->current_lens_samples_v;

    return ISTATUS_SUCCESS;
}

ISTATUS
GridPixelSamplerNextSample(
    _In_ const void *context,
    _Inout_ PRANDOM rng,
    _In_ size_t sample_index,
    _Out_ float_t *pixel_sample_u,
    _Out_ float_t *pixel_sample_v,
    _Out_ float_t *lens_sample_u,
    _Out_ float_t *lens_sample_v
    )
{
    PGRID_PIXEL_SAMPLER pixel_sampler = (PGRID_PIXEL_SAMPLER)context;

    float_t pixel_jitter_u, pixel_jitter_v;
    if (pixel_sampler->jitter_pixel_samples)
    {
        ISTATUS status = RandomGenerateFloat(rng,
                                             (float_t)0.0,
                                             pixel_sampler->pixel_sample_width_u,
                                             &pixel_jitter_u);

        if (status != ISTATUS_SUCCESS)
        {
            return status;
        }

        status = RandomGenerateFloat(rng,
                                     (float_t)0.0,
                                     pixel_sampler->pixel_sample_width_v,
                                     &pixel_jitter_v);

        if (status != ISTATUS_SUCCESS)
        {
            return status;
        }
    }
    else
    {
        pixel_jitter_u = (float_t)0.5 * pixel_sampler->pixel_sample_width_u;
        pixel_jitter_v = (float_t)0.5 * pixel_sampler->pixel_sample_width_v;
    }

    size_t pixel_u_index =
        sample_index % pixel_sampler->current_pixel_samples_u;
    pixel_jitter_u += fma((float_t)pixel_u_index,
                          pixel_sampler->pixel_sample_width_u,
                          pixel_sampler->pixel_min_u);
    *pixel_sample_u = fmax(fmin(pixel_jitter_u, pixel_sampler->pixel_max_u),
                           pixel_sampler->pixel_min_u);
    size_t sample_divisor = pixel_sampler->current_pixel_samples_u;

    size_t pixel_v_index =
        (sample_index / sample_divisor) % pixel_sampler->current_pixel_samples_v;
    pixel_jitter_v += fma((float_t)pixel_v_index,
                          pixel_sampler->pixel_sample_width_v,
                          pixel_sampler->pixel_min_v);
    *pixel_sample_v = fmax(fmin(pixel_jitter_v, pixel_sampler->pixel_max_v),
                           pixel_sampler->pixel_min_v);
    sample_divisor *= pixel_sampler->current_pixel_samples_v;

    float_t lens_jitter_u, lens_jitter_v;
    if (pixel_sampler->jitter_lens_samples)
    {
        ISTATUS status = RandomGenerateFloat(rng,
                                             (float_t)0.0,
                                             pixel_sampler->lens_sample_width_u,
                                             &lens_jitter_u);

        if (status != ISTATUS_SUCCESS)
        {
            return status;
        }

        status = RandomGenerateFloat(rng,
                                     (float_t)0.0,
                                     pixel_sampler->lens_sample_width_v,
                                     &lens_jitter_v);

        if (status != ISTATUS_SUCCESS)
        {
            return status;
        }
    }
    else
    {
        lens_jitter_u = (float_t)0.5 * pixel_sampler->lens_sample_width_u;
        lens_jitter_v = (float_t)0.5 * pixel_sampler->lens_sample_width_v;
    }

    size_t lens_u_index =
        (sample_index / sample_divisor) % pixel_sampler->current_lens_samples_u;
    lens_jitter_u += fma((float_t)lens_u_index,
                         pixel_sampler->lens_sample_width_u,
                         pixel_sampler->lens_min_u);
    *lens_sample_u = fmax(fmin(pixel_jitter_u, pixel_sampler->lens_max_u),
                           pixel_sampler->lens_min_u);
    sample_divisor *= pixel_sampler->current_lens_samples_u;

    size_t lens_v_index =
        (sample_index / sample_divisor) % pixel_sampler->current_lens_samples_v;
    lens_jitter_v += fma((float_t)lens_v_index,
                         pixel_sampler->lens_sample_width_v,
                         pixel_sampler->lens_min_v);
    *lens_sample_v = fmax(fmin(lens_jitter_v, pixel_sampler->lens_max_v),
                          pixel_sampler->lens_min_v);

    return ISTATUS_SUCCESS;
}

ISTATUS
GridPixelSamplerDuplicate(
    _In_opt_ const void *context,
    _Out_ PPIXEL_SAMPLER *duplicate
    )
{
    PCGRID_PIXEL_SAMPLER grid_pixel_sampler = (PCGRID_PIXEL_SAMPLER)context;

    ISTATUS status =
        GridPixelSamplerAllocate(grid_pixel_sampler->pixel_samples_u,
                                 grid_pixel_sampler->pixel_samples_v,
                                 grid_pixel_sampler->jitter_pixel_samples,
                                 grid_pixel_sampler->lens_samples_u,
                                 grid_pixel_sampler->lens_samples_v,
                                 grid_pixel_sampler->jitter_lens_samples,
                                 duplicate);

    return status;
}

//
// Static Variables
//

static const PIXEL_SAMPLER_VTABLE grid_pixel_sampler_vtable = {
    GridPixelSamplerPrepareSamples,
    GridPixelSamplerNextSample,
    GridPixelSamplerDuplicate,
    NULL
};

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
    )
{
    if (pixel_samples_u == 0)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }
    
    if (pixel_samples_v == 0)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }
    
    if (lens_samples_u == 0)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }
    
    if (lens_samples_v == 0)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    if (pixel_sampler == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_06;
    }

    size_t max_sample_count;
    bool success = CheckedMultiplySizeT(pixel_samples_u,
                                        pixel_samples_v,
                                        &max_sample_count);

    if (!success)
    {
        return ISTATUS_INVALID_ARGUMENT_COMBINATION_00;
    }

    success = CheckedMultiplySizeT(max_sample_count,
                                   lens_samples_u,
                                   &max_sample_count);

    if (!success)
    {
        return ISTATUS_INVALID_ARGUMENT_COMBINATION_00;
    }

    success = CheckedMultiplySizeT(max_sample_count,
                                   lens_samples_v,
                                   &max_sample_count);

    if (!success)
    {
        return ISTATUS_INVALID_ARGUMENT_COMBINATION_00;
    }

    GRID_PIXEL_SAMPLER grid_pixel_sampler;
    grid_pixel_sampler.pixel_samples_u = pixel_samples_u;
    grid_pixel_sampler.pixel_samples_v = pixel_samples_v;
    grid_pixel_sampler.lens_samples_u = lens_samples_u;
    grid_pixel_sampler.lens_samples_v = lens_samples_v;
    grid_pixel_sampler.pixel_min_u = (float_t)0.0;
    grid_pixel_sampler.pixel_min_v = (float_t)0.0;
    grid_pixel_sampler.lens_min_u = (float_t)0.0;
    grid_pixel_sampler.lens_min_v = (float_t)0.0;
    grid_pixel_sampler.pixel_max_u = (float_t)1.0;
    grid_pixel_sampler.pixel_max_v = (float_t)1.0;
    grid_pixel_sampler.lens_max_u = (float_t)1.0;
    grid_pixel_sampler.lens_max_v = (float_t)1.0;
    grid_pixel_sampler.pixel_sample_width_u = (float_t)0.0;
    grid_pixel_sampler.pixel_sample_width_v = (float_t)0.0;
    grid_pixel_sampler.lens_sample_width_u = (float_t)0.0;
    grid_pixel_sampler.lens_sample_width_v = (float_t)0.0;
    grid_pixel_sampler.jitter_pixel_samples = jitter_pixel_samples;
    grid_pixel_sampler.jitter_lens_samples = jitter_lens_samples;
    grid_pixel_sampler.current_pixel_samples_u = 1;
    grid_pixel_sampler.current_pixel_samples_v = 1;
    grid_pixel_sampler.current_lens_samples_u = 1;
    grid_pixel_sampler.current_lens_samples_v = 1;

    ISTATUS status = PixelSamplerAllocate(&grid_pixel_sampler_vtable,
                                          &grid_pixel_sampler,
                                          sizeof(GRID_PIXEL_SAMPLER),
                                          alignof(GRID_PIXEL_SAMPLER),
                                          pixel_sampler);

    return status;
}