/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    grid_pixel_sampler.c

Abstract:

    Implements a grid pixel sampler.

--*/

#include <stdalign.h>

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
    uint16_t pixel_index_u;
    uint16_t pixel_index_v;
    uint16_t lens_index_u;
    uint16_t lens_index_v;
    float_t pixel_current_u;
    float_t pixel_current_v;
    float_t lens_current_u;
    float_t lens_current_v;
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
    _In_ float_t lens_max_v
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

    pixel_sampler->pixel_sample_width_u = 
        (pixel_max_u - pixel_min_u) / (float_t)pixel_sampler->pixel_samples_u;
    pixel_sampler->pixel_sample_width_v = 
        (pixel_max_v - pixel_min_v) / (float_t)pixel_sampler->pixel_samples_v;
    pixel_sampler->lens_sample_width_u = 
        (lens_max_u - lens_min_u) / (float_t)pixel_sampler->lens_samples_u;
    pixel_sampler->lens_sample_width_v = 
        (lens_max_v - lens_min_v) / (float_t)pixel_sampler->lens_samples_v;

    pixel_sampler->pixel_index_u = 0;
    pixel_sampler->pixel_index_v = 0;
    pixel_sampler->lens_index_u = 0;
    pixel_sampler->lens_index_v = 0;
    pixel_sampler->pixel_current_u = pixel_min_u;
    pixel_sampler->pixel_current_v = pixel_min_v;
    pixel_sampler->lens_current_u = lens_min_u;
    pixel_sampler->lens_current_v = lens_min_v;

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == 0 || return == 1)
ISTATUS
GridPixelSamplerNextSample(
    _In_ void *context,
    _Inout_ PRANDOM rng,
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
                                             (float_t)1.0,
                                             &pixel_jitter_u);

        if (status != ISTATUS_SUCCESS)
        {
            return status;
        }

        status = RandomGenerateFloat(rng,
                                     (float_t)0.0,
                                     (float_t)1.0,
                                     &pixel_jitter_v);

        if (status != ISTATUS_SUCCESS)
        {
            return status;
        }
    }
    else
    {
        pixel_jitter_u = (float_t)0.5;
        pixel_jitter_v = (float_t)0.5;
    }

    *pixel_sample_u = fma(pixel_jitter_u,
                          pixel_sampler->pixel_sample_width_u,
                          pixel_sampler->pixel_current_u);
    *pixel_sample_u = fmin(*pixel_sample_u, (float_t)1.0);

    *pixel_sample_v = fma(pixel_jitter_v,
                          pixel_sampler->pixel_sample_width_v,
                          pixel_sampler->pixel_current_v);
    *pixel_sample_v = fmin(*pixel_sample_v, (float_t)1.0);

    float_t lens_jitter_u, lens_jitter_v;
    if (pixel_sampler->jitter_pixel_samples)
    {
        ISTATUS status = RandomGenerateFloat(rng,
                                             (float_t)0.0,
                                             (float_t)1.0,
                                             &lens_jitter_u);

        if (status != ISTATUS_SUCCESS)
        {
            return status;
        }

        status = RandomGenerateFloat(rng,
                                     (float_t)0.0,
                                     (float_t)1.0,
                                     &lens_jitter_v);

        if (status != ISTATUS_SUCCESS)
        {
            return status;
        }
    }
    else
    {
        lens_jitter_u = (float_t)0.5;
        lens_jitter_v = (float_t)0.5;
    }

    *lens_sample_u = fma(lens_jitter_u,
                         pixel_sampler->lens_sample_width_u,
                         pixel_sampler->lens_current_u);
    *lens_sample_u = fmin(*lens_sample_u, (float_t)1.0);

    *lens_sample_v = fma(lens_jitter_v,
                         pixel_sampler->lens_sample_width_v,
                         pixel_sampler->lens_current_v);
    *lens_sample_v = fmin(*lens_sample_v, (float_t)1.0);

    pixel_sampler->lens_index_v += 1;

    if (pixel_sampler->lens_index_v < pixel_sampler->current_lens_samples_v)
    {
        pixel_sampler->lens_current_v += pixel_sampler->lens_sample_width_v;
        return ISTATUS_SUCCESS;
    }

    pixel_sampler->lens_current_v = pixel_sampler->lens_min_v;
    pixel_sampler->lens_index_v = 0;

    pixel_sampler->lens_index_u += 1;

    if (pixel_sampler->lens_index_u < pixel_sampler->current_lens_samples_u)
    {
        pixel_sampler->lens_current_u += pixel_sampler->lens_sample_width_u;
        return ISTATUS_SUCCESS;
    }

    pixel_sampler->lens_current_u = pixel_sampler->lens_min_u;
    pixel_sampler->lens_index_u = 0;

    pixel_sampler->pixel_index_v += 1;

    if (pixel_sampler->pixel_index_v < pixel_sampler->current_pixel_samples_v)
    {
        pixel_sampler->pixel_current_v += pixel_sampler->pixel_sample_width_v;
        return ISTATUS_SUCCESS;
    }

    pixel_sampler->pixel_current_v = pixel_sampler->pixel_min_v;
    pixel_sampler->pixel_index_v = 0;

    pixel_sampler->pixel_index_u += 1;

    if (pixel_sampler->pixel_index_u < pixel_sampler->current_pixel_samples_u)
    {
        pixel_sampler->pixel_current_u += pixel_sampler->pixel_sample_width_u;
        return ISTATUS_SUCCESS;
    }

    return ISTATUS_DONE;
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

    GRID_PIXEL_SAMPLER grid_pixel_sampler;
    grid_pixel_sampler.pixel_samples_u = pixel_samples_u;
    grid_pixel_sampler.pixel_samples_v = pixel_samples_v;
    grid_pixel_sampler.lens_samples_u = lens_samples_u;
    grid_pixel_sampler.lens_samples_v = lens_samples_v;
    grid_pixel_sampler.pixel_min_u = (float_t)0.0;
    grid_pixel_sampler.pixel_min_v = (float_t)0.0;
    grid_pixel_sampler.lens_min_u = (float_t)0.0;
    grid_pixel_sampler.lens_min_v = (float_t)0.0;
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
    grid_pixel_sampler.pixel_index_u = 0;
    grid_pixel_sampler.pixel_index_v = 0;
    grid_pixel_sampler.lens_index_u = 0;
    grid_pixel_sampler.lens_index_v = 0;
    grid_pixel_sampler.pixel_current_u = (float_t)0.0;
    grid_pixel_sampler.pixel_current_v = (float_t)0.0;
    grid_pixel_sampler.lens_current_u = (float_t)0.0;
    grid_pixel_sampler.lens_current_v = (float_t)0.0;

    ISTATUS status = PixelSamplerAllocate(&grid_pixel_sampler_vtable,
                                          &grid_pixel_sampler,
                                          sizeof(GRID_PIXEL_SAMPLER),
                                          alignof(GRID_PIXEL_SAMPLER),
                                          pixel_sampler);

    return status;
}