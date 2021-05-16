/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    grid_image_sampler.c

Abstract:

    Implements a grid image sampler.

--*/

#include <stdalign.h>

#include "iris_camera_toolkit/grid_image_sampler.h"

//
// Types
//

typedef struct _GRID_IMAGE_SAMPLER {
    uint16_t pixel_samples_u;
    uint16_t pixel_samples_v;
    uint16_t lens_samples_u;
    uint16_t lens_samples_v;
    uint16_t subpixel_u_index;
    uint16_t subpixel_v_index;
    uint16_t lens_u_index;
    uint16_t lens_v_index;
    float_t pixel_u_base;
    float_t pixel_v_base;
    float_t subpixel_u_width;
    float_t subpixel_v_width;
    float_t sublens_u_width;
    float_t sublens_v_width;
    float_t subpixel_u_center;
    float_t subpixel_v_center;
    float_t sublens_u_center;
    float_t sublens_v_center;
    bool jitter_pixel_samples;
    bool jitter_lens_samples;
} GRID_IMAGE_SAMPLER, *PGRID_IMAGE_SAMPLER;

typedef const GRID_IMAGE_SAMPLER *PCGRID_IMAGE_SAMPLER;

//
// Static Functions
//

static
ISTATUS
GridImageSamplerStart(
    _In_ void *context,
    _In_ size_t column,
    _In_ size_t num_columns,
    _In_ size_t row,
    _In_ size_t num_rows,
    _Out_ uint32_t *num_samples
    )
{
    PGRID_IMAGE_SAMPLER image_sampler = (PGRID_IMAGE_SAMPLER)context;

    image_sampler->subpixel_u_index = 0;
    image_sampler->subpixel_v_index = 0;
    image_sampler->lens_u_index = 0;
    image_sampler->lens_v_index = 0;

    image_sampler->pixel_u_base = (float_t)column / (float_t)num_columns;
    image_sampler->pixel_v_base = (float_t)row / (float_t)num_rows;

    float_t pixel_u_width = (float_t)1.0 / (float_t)num_columns;
    image_sampler->subpixel_u_width =
        pixel_u_width / (float_t)image_sampler->pixel_samples_u;

    float_t pixel_v_width = (float_t)1.0 / (float_t)num_columns;
    image_sampler->subpixel_v_width =
        pixel_v_width / (float_t)image_sampler->pixel_samples_v;

    image_sampler->subpixel_u_center =
        (float_t)0.5 * image_sampler->subpixel_u_width;
    image_sampler->subpixel_v_center =
        (float_t)0.5 * image_sampler->subpixel_v_width;

    *num_samples = image_sampler->pixel_samples_u *
                   image_sampler->pixel_samples_v *
                   image_sampler->lens_samples_u *
                   image_sampler->lens_samples_v;

    return ISTATUS_SUCCESS;
}

static
ISTATUS
GridImageSamplerNext(
    _In_ void *context,
    _Inout_ PRANDOM rng,
    _Out_ float_t *pixel_u,
    _Out_ float_t *pixel_v,
    _Out_ float_t *dpixel_u,
    _Out_ float_t *dpixel_v,
    _Out_opt_ float_t *lens_u,
    _Out_opt_ float_t *lens_v
    )
{
    PGRID_IMAGE_SAMPLER image_sampler = (PGRID_IMAGE_SAMPLER)context;

    if (image_sampler->jitter_pixel_samples)
    {
        ISTATUS status = RandomGenerateFloat(rng,
                                             (float_t)0.0,
                                             image_sampler->subpixel_u_width,
                                             pixel_u);

        if (status != ISTATUS_SUCCESS)
        {
            return status;
        }

        status = RandomGenerateFloat(rng,
                                     (float_t)0.0,
                                     image_sampler->subpixel_v_width,
                                     pixel_v);

        if (status != ISTATUS_SUCCESS)
        {
            return status;
        }
    }
    else
    {
        *pixel_u = image_sampler->subpixel_u_center;
        *pixel_v = image_sampler->subpixel_v_center;
    }

    *pixel_u += fma((float_t)image_sampler->subpixel_u_index,
                     image_sampler->subpixel_u_width,
                     image_sampler->pixel_u_base);

    *pixel_v += fma((float_t)image_sampler->subpixel_v_index,
                     image_sampler->subpixel_v_width,
                     image_sampler->pixel_v_base);

    *dpixel_u = image_sampler->subpixel_u_width;
    *dpixel_v = image_sampler->subpixel_v_width;

    if (lens_u != NULL)
    {
        if (image_sampler->jitter_lens_samples)
        {
            ISTATUS status = RandomGenerateFloat(rng,
                                                (float_t)0.0,
                                                image_sampler->sublens_u_width,
                                                lens_u);

            if (status != ISTATUS_SUCCESS)
            {
                return status;
            }
        }
        else
        {
            *lens_u = image_sampler->sublens_u_center;
        }

        *lens_u += (float_t)image_sampler->subpixel_u_index *
                image_sampler->subpixel_u_width;
    }

    if (lens_v != NULL)
    {
        if (image_sampler->jitter_lens_samples)
        {
            ISTATUS status = RandomGenerateFloat(rng,
                                                (float_t)0.0,
                                                image_sampler->sublens_v_width,
                                                lens_v);

            if (status != ISTATUS_SUCCESS)
            {
                return status;
            }
        }
        else
        {
            *lens_v = image_sampler->sublens_v_center;
        }

        *lens_v += (float_t)image_sampler->subpixel_v_index *
                image_sampler->subpixel_v_width;
    }

    image_sampler->subpixel_u_index += 1;

    if (image_sampler->subpixel_u_index != image_sampler->pixel_samples_u)
    {
        return ISTATUS_SUCCESS;
    }

    image_sampler->subpixel_u_index = 0;
    image_sampler->subpixel_v_index += 1;

    if (image_sampler->subpixel_v_index != image_sampler->pixel_samples_v)
    {
        return ISTATUS_SUCCESS;
    }

    image_sampler->subpixel_v_index = 0;
    image_sampler->lens_u_index += 1;

    if (image_sampler->lens_u_index != image_sampler->lens_samples_u)
    {
        return ISTATUS_SUCCESS;
    }

    image_sampler->lens_u_index = 0;
    image_sampler->lens_v_index += 1;

    return ISTATUS_SUCCESS;
}

static
ISTATUS
GridImageSamplerDuplicate(
    _In_opt_ const void *context,
    _Out_ PIMAGE_SAMPLER *duplicate
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
                                 duplicate);

    return status;
}

//
// Static Variables
//

static const IMAGE_SAMPLER_VTABLE grid_image_sampler_vtable = {
    NULL,
    NULL,
    GridImageSamplerStart,
    GridImageSamplerNext,
    GridImageSamplerDuplicate,
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

    uint64_t max_sample_count =
        pixel_samples_u * pixel_samples_v * lens_samples_u * lens_samples_v;

    if (max_sample_count > UINT32_MAX)
    {
        return ISTATUS_INVALID_ARGUMENT_COMBINATION_00;
    }

    GRID_IMAGE_SAMPLER grid_image_sampler;
    grid_image_sampler.pixel_samples_u = pixel_samples_u;
    grid_image_sampler.pixel_samples_v = pixel_samples_v;
    grid_image_sampler.lens_samples_u = lens_samples_u;
    grid_image_sampler.lens_samples_v = lens_samples_v;
    grid_image_sampler.sublens_u_width = (float_t)1.0 / (float_t)lens_samples_u;
    grid_image_sampler.sublens_v_width = (float_t)1.0 / (float_t)lens_samples_v;
    grid_image_sampler.sublens_u_center =
        (float_t)0.5 * grid_image_sampler.sublens_u_width;
    grid_image_sampler.sublens_v_center =
        (float_t)0.5 * grid_image_sampler.sublens_v_width;
    grid_image_sampler.jitter_pixel_samples = jitter_pixel_samples;
    grid_image_sampler.jitter_lens_samples = jitter_lens_samples;

    ISTATUS status = ImageSamplerAllocate(&grid_image_sampler_vtable,
                                          &grid_image_sampler,
                                          sizeof(GRID_IMAGE_SAMPLER),
                                          alignof(GRID_IMAGE_SAMPLER),
                                          image_sampler);

    return status;
}