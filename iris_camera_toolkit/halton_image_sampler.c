/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    halton_image_sampler.c

Abstract:

    Implements a halton image sampler.

--*/

#include <float.h>
#include <stdalign.h>
#include <string.h>
#include <stdlib.h>

#include "common/safe_math.h"
#include "iris_camera_toolkit/halton_image_sampler.h"
#include "third_party/gruenschloss/halton/halton_enum.h"
#include "third_party/gruenschloss/halton/halton_sampler.h"

//
// Halton RNG Types
//

typedef struct _HALTON_RANDOM_DATA {
    const Halton_sampler* halton_sampler;
    unsigned sample_index;
    unsigned dimension;
} HALTON_RANDOM_DATA, *PHALTON_RANDOM_DATA;

typedef struct _HALTON_RANDOM {
    PHALTON_RANDOM_DATA data;
    bool owned;
} HALTON_RANDOM, *PHALTON_RANDOM;

//
// Halton RNG Forward Declarations
//

static
ISTATUS
HaltonRandomAllocate(
    _In_ PHALTON_RANDOM_DATA data,
    _In_ bool owned,
    _Out_ PRANDOM *rng
    );

//
// Halton RNG Static Functions
//

static
ISTATUS
HaltonRandomGenerateFloat(
    _In_ void *context,
    _In_ float_t minimum,
    _In_ float_t maximum,
    _Out_range_(minimum, maximum) float_t *result
    )
{
    PHALTON_RANDOM halton_random = (PHALTON_RANDOM)context;

    float value;
    bool success = checked_sample(halton_random->data->halton_sampler,
                                  halton_random->data->dimension,
                                  halton_random->data->sample_index,
                                  &value);

    if (!success)
    {
        return ISTATUS_OUT_OF_ENTROPY;
    }

    halton_random->data->dimension += 1;

    *result = minimum + (maximum - minimum) * (float_t)value;

    return ISTATUS_SUCCESS;
}

static
ISTATUS
HaltonRandomGenerateIndex(
    _In_ void *context,
    _In_ size_t upper_bound,
    _Out_range_(0, upper_bound - 1) size_t *result
    )
{
    PHALTON_RANDOM halton_random = (PHALTON_RANDOM)context;

    float value;
    bool success = checked_sample(halton_random->data->halton_sampler,
                                  halton_random->data->dimension,
                                  halton_random->data->sample_index,
                                  &value);

    if (!success)
    {
        return ISTATUS_OUT_OF_ENTROPY;
    }

    halton_random->data->dimension += 1;

    *result = (float_t)upper_bound * (float_t)value;

    return ISTATUS_SUCCESS;
}

static
ISTATUS
HaltonRandomReplicate(
    _In_opt_ void *context,
    _Out_ PRANDOM *replica
    )
{
    PHALTON_RANDOM halton_random = (PHALTON_RANDOM)context;

    PHALTON_RANDOM_DATA data =
        (PHALTON_RANDOM_DATA)malloc(sizeof(HALTON_RANDOM_DATA));

    if (data == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    memcpy(data, halton_random->data, sizeof(HALTON_RANDOM_DATA));

    ISTATUS status = HaltonRandomAllocate(data,
                                          true,
                                          replica);

    return status;
}

static
void
HaltonRandomFree(
    _In_opt_ _Post_invalid_ void *context
    )
{
    PHALTON_RANDOM halton_random = (PHALTON_RANDOM)context;

    if (halton_random->owned)
    {
        free(halton_random->data);
    }
}

//
// Halton RNG Static Variables
//

static const RANDOM_VTABLE halton_vtable = {
    HaltonRandomGenerateFloat,
    HaltonRandomGenerateIndex,
    HaltonRandomReplicate,
    HaltonRandomFree
};

//
// Halton RNG Static Functions
//

static
ISTATUS
HaltonRandomAllocate(
    _In_ PHALTON_RANDOM_DATA data,
    _In_ bool owned,
    _Out_ PRANDOM *rng
    )
{
    assert(data != NULL);
    assert(rng != NULL);

    HALTON_RANDOM halton;
    halton.data = data;
    halton.owned = owned;

    ISTATUS status = RandomAllocate(&halton_vtable,
                                    &halton,
                                    sizeof(HALTON_RANDOM),
                                    alignof(HALTON_RANDOM),
                                    rng);

    return status;
}

//
// Types
//

typedef struct _HALTON_IMAGE_SAMPLER {
    Halton_sampler halton_sampler;
    Halton_enum halton_enum;
    HALTON_RANDOM_DATA random_data;
    unsigned current_sample_index;
    uint32_t samples_per_pixel;
    double_t to_pixel_u;
    double_t to_pixel_v;
    float_t lens_min_u;
    float_t lens_delta_u;
    float_t lens_min_v;
    float_t lens_delta_v;
    float_t dpixel_sample_u;
    float_t dpixel_sample_v;
} HALTON_IMAGE_SAMPLER, *PHALTON_IMAGE_SAMPLER;

typedef const HALTON_IMAGE_SAMPLER *PCHALTON_IMAGE_SAMPLER;

//
// Static Functions
//

static
ISTATUS
HaltonImageSamplerPrepareImageSamples(
    _In_ void *context,
    _In_ size_t num_columns,
    _In_ size_t num_rows
    )
{
    PHALTON_IMAGE_SAMPLER image_sampler = (PHALTON_IMAGE_SAMPLER)context;

    image_sampler->halton_enum = halton_enum((unsigned int)num_columns,
                                             (unsigned int)num_rows);

    image_sampler->to_pixel_u =
        (double_t)image_sampler->halton_enum.m_scale_x / (double_t)num_columns;
    image_sampler->to_pixel_v =
        (double_t)image_sampler->halton_enum.m_scale_y / (double_t)num_rows;

    float_t sqrt_samples = sqrt((float_t)image_sampler->samples_per_pixel);
    image_sampler->dpixel_sample_u =
        (float_t)1.0 / ((float_t)num_columns * sqrt_samples);
    image_sampler->dpixel_sample_v =
        (float_t)1.0 / ((float_t)num_rows * sqrt_samples);

    return ISTATUS_SUCCESS;
}

static
ISTATUS
HaltonImageSamplerPrepareRandom(
    _In_ void *context,
    _Out_ PRANDOM *rng
    )
{
    PHALTON_IMAGE_SAMPLER image_sampler = (PHALTON_IMAGE_SAMPLER)context;

    image_sampler->random_data.halton_sampler = &image_sampler->halton_sampler;
    image_sampler->random_data.sample_index = 0;
    image_sampler->random_data.dimension = 0;

    ISTATUS status = HaltonRandomAllocate(&image_sampler->random_data,
                                          false,
                                          rng);

    return status;
}

static
ISTATUS
HaltonImageSamplerPreparePixelSamples(
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
    PHALTON_IMAGE_SAMPLER image_sampler = (PHALTON_IMAGE_SAMPLER)context;

    image_sampler->current_sample_index = get_index(&image_sampler->halton_enum,
                                                    0,
                                                    column,
                                                    row);

    image_sampler->lens_min_u = lens_min_u;
    image_sampler->lens_min_v = lens_min_v;
    image_sampler->lens_delta_u = lens_max_u - lens_min_u;
    image_sampler->lens_delta_v = lens_max_v - lens_min_v;

    *num_samples = (size_t)image_sampler->samples_per_pixel;

    return ISTATUS_SUCCESS;
}

static
ISTATUS
HaltonImageSamplerNextSample(
    _In_ const void *context,
    _Inout_ PRANDOM pixel_rng,
    _In_ size_t sample_index,
    _Out_ float_t *pixel_sample_u,
    _Out_ float_t *pixel_sample_v,
    _Out_ float_t *lens_sample_u,
    _Out_ float_t *lens_sample_v,
    _Out_ float_t *dpixel_sample_u,
    _Out_ float_t *dpixel_sample_v
    )
{
    PHALTON_IMAGE_SAMPLER image_sampler = (PHALTON_IMAGE_SAMPLER)context;

    image_sampler->random_data.sample_index =
        image_sampler->current_sample_index +
        image_sampler->halton_enum.m_increment * (unsigned)sample_index;
    image_sampler->random_data.dimension = 0;

    float_t halton_u;
    ISTATUS status = RandomGenerateFloat(pixel_rng,
                                         (float_t)0.0,
                                         (float_t)1.0,
                                         &halton_u);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    *pixel_sample_u = (float_t)(halton_u * image_sampler->to_pixel_u);

    float_t halton_v;
    status = RandomGenerateFloat(pixel_rng,
                                 (float_t)0.0,
                                 (float_t)1.0,
                                 &halton_v);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    *pixel_sample_v = (float_t)(halton_v * image_sampler->to_pixel_v);

    if (image_sampler->lens_delta_u != (float_t)0.0)
    {
        float_t value;
        status = RandomGenerateFloat(pixel_rng,
                                     image_sampler->lens_min_u,
                                     image_sampler->lens_min_u + image_sampler->lens_delta_u,
                                     &value);

        if (status != ISTATUS_SUCCESS)
        {
            return status;
        }
    }
    else
    {
        *lens_sample_u = image_sampler->lens_min_u;
    }

    if (image_sampler->lens_delta_v != (float_t)0.0)
    {
        float_t value;
        status = RandomGenerateFloat(pixel_rng,
                                     image_sampler->lens_min_v,
                                     image_sampler->lens_min_v + image_sampler->lens_delta_v,
                                     &value);

        if (status != ISTATUS_SUCCESS)
        {
            return status;
        }
    }
    else
    {
        *lens_sample_v = image_sampler->lens_min_v;
    }

    *dpixel_sample_u = image_sampler->dpixel_sample_u;
    *dpixel_sample_v = image_sampler->dpixel_sample_v;

    return ISTATUS_SUCCESS;
}

static
ISTATUS
HaltonImageSamplerReplicate(
    _In_opt_ const void *context,
    _Out_ PIMAGE_SAMPLER *replica
    );

//
// Static Variables
//

static const IMAGE_SAMPLER_VTABLE halton_image_sampler_vtable = {
    HaltonImageSamplerPrepareImageSamples,
    NULL,
    HaltonImageSamplerPrepareRandom,
    HaltonImageSamplerPreparePixelSamples,
    HaltonImageSamplerNextSample,
    HaltonImageSamplerReplicate,
    NULL
};

//
// Static Functions
//

static
ISTATUS
HaltonImageSamplerReplicate(
    _In_opt_ const void *context,
    _Out_ PIMAGE_SAMPLER *replica
    )
{
    PCHALTON_IMAGE_SAMPLER halton_image_sampler = (PCHALTON_IMAGE_SAMPLER)context;

    ISTATUS status = ImageSamplerAllocate(&halton_image_sampler_vtable,
                                          halton_image_sampler,
                                          sizeof(HALTON_IMAGE_SAMPLER),
                                          alignof(HALTON_IMAGE_SAMPLER),
                                          replica);

    return status;
}

//
// Functions
//

ISTATUS
HaltonImageSamplerAllocate(
    _In_ uint32_t samples_per_pixel,
    _Out_ PIMAGE_SAMPLER *image_sampler
    )
{
    if (samples_per_pixel == 0)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (image_sampler == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    HALTON_IMAGE_SAMPLER halton_image_sampler;
    halton_image_sampler.samples_per_pixel = samples_per_pixel;

    init_faure(&halton_image_sampler.halton_sampler);

    ISTATUS status = ImageSamplerAllocate(&halton_image_sampler_vtable,
                                          &halton_image_sampler,
                                          sizeof(HALTON_IMAGE_SAMPLER),
                                          alignof(HALTON_IMAGE_SAMPLER),
                                          image_sampler);

    return status;
}