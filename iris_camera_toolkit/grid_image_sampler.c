/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    grid_image_sampler.c

Abstract:

    Implements a grid image sampler.

--*/

#include <stdalign.h>

#include "common/safe_math.h"
#include "iris_camera_toolkit/grid_image_sampler.h"

//
// Types
//

typedef struct _GRID_IMAGE_SAMPLER {
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
} GRID_IMAGE_SAMPLER, *PGRID_IMAGE_SAMPLER;

typedef const GRID_IMAGE_SAMPLER *PCGRID_IMAGE_SAMPLER;

//
// Static Functions
//

static
ISTATUS
GridImageSamplerPreparePixelSamples(
    _In_ void *context,
    _In_ size_t column,
    _In_ size_t row,
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
    PGRID_IMAGE_SAMPLER image_sampler = (PGRID_IMAGE_SAMPLER)context;

    if (pixel_min_u == pixel_max_u)
    {
        image_sampler->current_pixel_samples_u = 1;
    }
    else
    {
        image_sampler->current_pixel_samples_u =
            image_sampler->pixel_samples_u;
    }

    if (pixel_min_v == pixel_max_v)
    {
        image_sampler->current_pixel_samples_v = 1;
    }
    else
    {
        image_sampler->current_pixel_samples_v =
            image_sampler->pixel_samples_v;
    }

    if (lens_min_u == lens_max_u)
    {
        image_sampler->current_lens_samples_u = 1;
    }
    else
    {
        image_sampler->current_lens_samples_u =
            image_sampler->lens_samples_u;
    }

    if (lens_min_v == lens_max_v)
    {
        image_sampler->current_lens_samples_v = 1;
    }
    else
    {
        image_sampler->current_lens_samples_v =
            image_sampler->lens_samples_v;
    }

    image_sampler->pixel_min_u = pixel_min_u;
    image_sampler->pixel_min_v = pixel_min_v;
    image_sampler->lens_min_u = lens_min_u;
    image_sampler->lens_min_v = lens_min_v;
    image_sampler->pixel_max_u = pixel_max_u;
    image_sampler->pixel_max_v = pixel_max_v;
    image_sampler->lens_max_u = lens_max_u;
    image_sampler->lens_max_v = lens_max_v;

    image_sampler->pixel_sample_width_u =
        (pixel_max_u - pixel_min_u) / (float_t)image_sampler->pixel_samples_u;
    image_sampler->pixel_sample_width_v =
        (pixel_max_v - pixel_min_v) / (float_t)image_sampler->pixel_samples_v;
    image_sampler->lens_sample_width_u =
        (lens_max_u - lens_min_u) / (float_t)image_sampler->lens_samples_u;
    image_sampler->lens_sample_width_v =
        (lens_max_v - lens_min_v) / (float_t)image_sampler->lens_samples_v;

    *num_samples = image_sampler->current_pixel_samples_u *
                   image_sampler->current_pixel_samples_v *
                   image_sampler->current_lens_samples_u *
                   image_sampler->current_lens_samples_v;

    return ISTATUS_SUCCESS;
}

static
ISTATUS
GridImageSamplerNextSample(
    _In_ const void *context,
    _Inout_ PRANDOM rng,
    _In_ size_t sample_index,
    _Out_ float_t *pixel_sample_u,
    _Out_ float_t *pixel_sample_v,
    _Out_ float_t *lens_sample_u,
    _Out_ float_t *lens_sample_v,
    _Out_ float_t *dpixel_sample_u,
    _Out_ float_t *dpixel_sample_v
    )
{
    PGRID_IMAGE_SAMPLER image_sampler = (PGRID_IMAGE_SAMPLER)context;

    float_t pixel_jitter_u, pixel_jitter_v;
    if (image_sampler->jitter_pixel_samples)
    {
        ISTATUS status = RandomGenerateFloat(rng,
                                             (float_t)0.0,
                                             image_sampler->pixel_sample_width_u,
                                             &pixel_jitter_u);

        if (status != ISTATUS_SUCCESS)
        {
            return status;
        }

        status = RandomGenerateFloat(rng,
                                     (float_t)0.0,
                                     image_sampler->pixel_sample_width_v,
                                     &pixel_jitter_v);

        if (status != ISTATUS_SUCCESS)
        {
            return status;
        }
    }
    else
    {
        pixel_jitter_u = (float_t)0.5 * image_sampler->pixel_sample_width_u;
        pixel_jitter_v = (float_t)0.5 * image_sampler->pixel_sample_width_v;
    }

    size_t pixel_u_index =
        sample_index % image_sampler->current_pixel_samples_u;
    pixel_jitter_u +=
        image_sampler->pixel_min_u + (float_t)pixel_u_index * image_sampler->pixel_sample_width_u;
    *pixel_sample_u = IMax(IMin(pixel_jitter_u, image_sampler->pixel_max_u),
                           image_sampler->pixel_min_u);
    size_t sample_divisor = image_sampler->current_pixel_samples_u;

    size_t pixel_v_index =
        (sample_index / sample_divisor) % image_sampler->current_pixel_samples_v;
    pixel_jitter_v +=
        image_sampler->pixel_min_v + (float_t)pixel_v_index * image_sampler->pixel_sample_width_v;
    *pixel_sample_v = IMax(IMin(pixel_jitter_v, image_sampler->pixel_max_v),
                           image_sampler->pixel_min_v);
    sample_divisor *= image_sampler->current_pixel_samples_v;

    float_t lens_jitter_u, lens_jitter_v;
    if (image_sampler->jitter_lens_samples)
    {
        ISTATUS status = RandomGenerateFloat(rng,
                                             (float_t)0.0,
                                             image_sampler->lens_sample_width_u,
                                             &lens_jitter_u);

        if (status != ISTATUS_SUCCESS)
        {
            return status;
        }

        status = RandomGenerateFloat(rng,
                                     (float_t)0.0,
                                     image_sampler->lens_sample_width_v,
                                     &lens_jitter_v);

        if (status != ISTATUS_SUCCESS)
        {
            return status;
        }
    }
    else
    {
        lens_jitter_u = (float_t)0.5 * image_sampler->lens_sample_width_u;
        lens_jitter_v = (float_t)0.5 * image_sampler->lens_sample_width_v;
    }

    size_t lens_u_index =
        (sample_index / sample_divisor) % image_sampler->current_lens_samples_u;
    lens_jitter_u +=
        image_sampler->lens_min_u + (float_t)lens_u_index * image_sampler->lens_sample_width_u;
    *lens_sample_u = IMax(IMin(pixel_jitter_u, image_sampler->lens_max_u),
                           image_sampler->lens_min_u);
    sample_divisor *= image_sampler->current_lens_samples_u;

    size_t lens_v_index =
        (sample_index / sample_divisor) % image_sampler->current_lens_samples_v;
    lens_jitter_v +=
        image_sampler->lens_min_v + (float_t)lens_v_index * image_sampler->lens_sample_width_v;
    *lens_sample_v = IMax(IMin(lens_jitter_v, image_sampler->lens_max_v),
                          image_sampler->lens_min_v);

    *dpixel_sample_u = image_sampler->pixel_sample_width_u;
    *dpixel_sample_v = image_sampler->pixel_sample_width_v;

    return ISTATUS_SUCCESS;
}

static
ISTATUS
GridImageSamplerReplicate(
    _In_opt_ const void *context,
    _Out_ PIMAGE_SAMPLER *replica
    )
{
    PCGRID_IMAGE_SAMPLER grid_image_sampler = (PCGRID_IMAGE_SAMPLER)context;

    ISTATUS status =
        GridImageSamplerAllocate(grid_image_sampler->pixel_samples_u,
                                 grid_image_sampler->pixel_samples_v,
                                 grid_image_sampler->jitter_pixel_samples,
                                 grid_image_sampler->lens_samples_u,
                                 grid_image_sampler->lens_samples_v,
                                 grid_image_sampler->jitter_lens_samples,
                                 replica);

    return status;
}

//
// Static Variables
//

static const IMAGE_SAMPLER_VTABLE grid_image_sampler_vtable = {
    NULL,
    NULL,
    NULL,
    GridImageSamplerPreparePixelSamples,
    GridImageSamplerNextSample,
    GridImageSamplerReplicate,
    NULL
};

//
// Functions
//

ISTATUS
GridImageSamplerAllocate(
    _In_ uint16_t pixel_samples_u,
    _In_ uint16_t pixel_samples_v,
    _In_ bool jitter_pixel_samples,
    _In_ uint16_t lens_samples_u,
    _In_ uint16_t lens_samples_v,
    _In_ bool jitter_lens_samples,
    _Out_ PIMAGE_SAMPLER *image_sampler
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

    if (image_sampler == NULL)
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

    GRID_IMAGE_SAMPLER grid_image_sampler;
    grid_image_sampler.pixel_samples_u = pixel_samples_u;
    grid_image_sampler.pixel_samples_v = pixel_samples_v;
    grid_image_sampler.lens_samples_u = lens_samples_u;
    grid_image_sampler.lens_samples_v = lens_samples_v;
    grid_image_sampler.pixel_min_u = (float_t)0.0;
    grid_image_sampler.pixel_min_v = (float_t)0.0;
    grid_image_sampler.lens_min_u = (float_t)0.0;
    grid_image_sampler.lens_min_v = (float_t)0.0;
    grid_image_sampler.pixel_max_u = (float_t)1.0;
    grid_image_sampler.pixel_max_v = (float_t)1.0;
    grid_image_sampler.lens_max_u = (float_t)1.0;
    grid_image_sampler.lens_max_v = (float_t)1.0;
    grid_image_sampler.pixel_sample_width_u = (float_t)0.0;
    grid_image_sampler.pixel_sample_width_v = (float_t)0.0;
    grid_image_sampler.lens_sample_width_u = (float_t)0.0;
    grid_image_sampler.lens_sample_width_v = (float_t)0.0;
    grid_image_sampler.jitter_pixel_samples = jitter_pixel_samples;
    grid_image_sampler.jitter_lens_samples = jitter_lens_samples;
    grid_image_sampler.current_pixel_samples_u = 1;
    grid_image_sampler.current_pixel_samples_v = 1;
    grid_image_sampler.current_lens_samples_u = 1;
    grid_image_sampler.current_lens_samples_v = 1;

    ISTATUS status = ImageSamplerAllocate(&grid_image_sampler_vtable,
                                          &grid_image_sampler,
                                          sizeof(GRID_IMAGE_SAMPLER),
                                          alignof(GRID_IMAGE_SAMPLER),
                                          image_sampler);

    return status;
}