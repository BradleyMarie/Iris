/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    sobol_image_sampler.c

Abstract:

    Implements a sobol image sampler.

--*/

#include <stdalign.h>

#include "iris_camera_toolkit/sobol_image_sampler.h"
#include "third_party/gruenschloss/double/sobol.h"
#include "third_party/pbrt-v3/sobolmatrices.h"

//
// Defines
//

#define PIXEL_U_DIMENSION 0
#define PIXEL_V_DIMENSION 1
#define LENS_U_DIMENSION 2
#define LENS_V_DIMENSION 3

//
// Types
//

typedef struct _SOBOL_IMAGE_SAMPLER {
    double to_pixel_u;
    double to_pixel_v;
    uint64_t logical_resolution_dimension;
    uint64_t logical_resolution_dimension_log2;
    size_t num_columns;
    size_t num_rows;
    size_t current_column;
    size_t current_row;
    float_t lens_min_u;
    float_t lens_delta_u;
    float_t lens_min_v;
    float_t lens_delta_v;
    float_t dpixel_sample_u;
    float_t dpixel_sample_v;
    uint32_t samples_per_pixel;
} SOBOL_IMAGE_SAMPLER, *PSOBOL_IMAGE_SAMPLER;

typedef const SOBOL_IMAGE_SAMPLER *PCSOBOL_IMAGE_SAMPLER;

//
// Static Functions
//

static
inline
uint64_t
RoundUpToPowerOfTwo(
    _In_ size_t value
    )
{
    uint64_t result = (uint64_t)value;

    result -= 1;
    result |= result >> 1;
    result |= result >> 2;
    result |= result >> 4;
    result |= result >> 8;
    result |= result >> 16;
    result |= result >> 32;
    result += 1;

    return result;
}

static
inline
uint64_t
UInt64Log2(
    _In_ uint64_t value
    )
{
    assert((value & (value - 1)) == 0);
    uint64_t mask = (uint64_t)1 << 63;

    uint64_t result = 63;
    for (size_t i = 0; i < 63; i++)
    {
        if (value & mask)
        {
            return result;
        }

        value <<= 1;
        result -= 1;
    }

    assert(false);
    return 0;
}

static
inline
uint64_t
BitMatrixVectorMultiply(
    _In_ const uint64_t bit_matrix[SOBOL_MATRIX_SIZE],
    _In_ uint64_t bit_vector
    )
{
    uint64_t product = 0;
    for (size_t matrix_column = 0;
         bit_vector;
         bit_vector >>= 1, matrix_column += 1)
    {
        assert(matrix_column < SOBOL_MATRIX_SIZE);

        if (bit_vector & 1)
        {
            product ^= bit_matrix[matrix_column];
        }
    }

    return product;
}

static
inline
uint64_t
SobolSequenceIndex(
    _In_ uint64_t resolution_dimension_log2,
    _In_ uint64_t pixel_column,
    _In_ uint64_t pixel_row,
    _In_ uint64_t pixel_sample_index
    )
{
    if (resolution_dimension_log2 == 0)
    {
        return pixel_sample_index;
    }

    size_t matrix_index = (size_t)resolution_dimension_log2 - 1;

    uint64_t transformed_pixel_sample_index =
        BitMatrixVectorMultiply(VdCSobolMatrices[matrix_index],
                                pixel_sample_index);

    uint64_t pixel_index = (pixel_column << resolution_dimension_log2) |
                           pixel_row;
    uint64_t image_sample_index = pixel_index ^ transformed_pixel_sample_index;        

    uint64_t transformed_image_sample_index =
        BitMatrixVectorMultiply(VdCSobolMatricesInv[matrix_index],
                                image_sample_index);

    uint64_t num_pixels_log_2 = resolution_dimension_log2 << 1;
    uint64_t sample_index_base = pixel_sample_index << num_pixels_log_2;

    return sample_index_base ^ transformed_image_sample_index;
}

static
ISTATUS
SobolImageSamplerPrepareImageSamples(
    _In_ void *context,
    _Inout_ PRANDOM rng,
    _In_ size_t num_columns,
    _In_ size_t num_rows
    )
{
    PSOBOL_IMAGE_SAMPLER image_sampler = (PSOBOL_IMAGE_SAMPLER)context;

    size_t max_dimension = (num_columns < num_rows) ? num_rows : num_columns;
    image_sampler->logical_resolution_dimension =
        RoundUpToPowerOfTwo(max_dimension);
    image_sampler->logical_resolution_dimension_log2 =
        UInt64Log2(image_sampler->logical_resolution_dimension);
    image_sampler->num_columns = num_columns;
    image_sampler->num_rows = num_rows;

    float_t sqrt_samples = sqrt((float_t)image_sampler->samples_per_pixel);
    image_sampler->dpixel_sample_u =
        (float_t)1.0 / ((float_t)num_columns * sqrt_samples);
    image_sampler->dpixel_sample_v =
        (float_t)1.0 / ((float_t)num_rows * sqrt_samples);

    return ISTATUS_SUCCESS;
}

static
ISTATUS
SobolImageSamplerPreparePixelSamples(
    _In_ void *context,
    _Inout_ PRANDOM rng,
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
    PSOBOL_IMAGE_SAMPLER image_sampler = (PSOBOL_IMAGE_SAMPLER)context;

    image_sampler->to_pixel_u =
        (double)image_sampler->logical_resolution_dimension /
        (double)image_sampler->num_columns;
    image_sampler->to_pixel_v =
        (double)image_sampler->logical_resolution_dimension /
        (double)image_sampler->num_rows;

    image_sampler->current_column = column;
    image_sampler->current_row = row;
    image_sampler->lens_min_u = lens_min_u;
    image_sampler->lens_min_v = lens_min_v;
    image_sampler->lens_delta_u = lens_max_u - lens_min_u;
    image_sampler->lens_delta_v = lens_max_v - lens_min_v;

    *num_samples = (size_t)image_sampler->samples_per_pixel;

    return ISTATUS_SUCCESS;
}

static
ISTATUS
SobolImageSamplerNextSample(
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
    PSOBOL_IMAGE_SAMPLER image_sampler = (PSOBOL_IMAGE_SAMPLER)context;

    uint64_t sobol_sample_index =
        SobolSequenceIndex(image_sampler->logical_resolution_dimension_log2,
                           image_sampler->current_column,
                           image_sampler->current_row,
                           sample_index);

    *pixel_sample_u = (float_t)(image_sampler->to_pixel_u *
        sobol_double_sample(sobol_sample_index,
                            PIXEL_U_DIMENSION,
                            SOBOL_DOUBLE_DEFAULT_SCRAMBLE));

    *pixel_sample_v = (float_t)(image_sampler->to_pixel_v *
        sobol_double_sample(sobol_sample_index,
                            PIXEL_V_DIMENSION,
                            SOBOL_DOUBLE_DEFAULT_SCRAMBLE));

    if (image_sampler->lens_delta_u != (float_t)0.0)
    {
        float_t value =
            (float_t)sobol_double_sample(sobol_sample_index,
                                         LENS_U_DIMENSION,
                                         SOBOL_DOUBLE_DEFAULT_SCRAMBLE);
        *lens_sample_u =
            image_sampler->lens_min_u + image_sampler->lens_delta_u * value;
    }
    else
    {
        *lens_sample_u = image_sampler->lens_min_u;
    }

    if (image_sampler->lens_delta_v != (float_t)0.0)
    {
        float_t value =
            (float_t)sobol_double_sample(sobol_sample_index,
                                         LENS_V_DIMENSION,
                                         SOBOL_DOUBLE_DEFAULT_SCRAMBLE);
        *lens_sample_v =
            image_sampler->lens_min_v + image_sampler->lens_delta_v * value;
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
SobolImageSamplerDuplicate(
    _In_opt_ const void *context,
    _Out_ PIMAGE_SAMPLER *duplicate
    );

//
// Static Variables
//

static const IMAGE_SAMPLER_VTABLE sobol_image_sampler_vtable = {
    SobolImageSamplerPrepareImageSamples,
    SobolImageSamplerPreparePixelSamples,
    SobolImageSamplerNextSample,
    SobolImageSamplerDuplicate,
    NULL
};

//
// Static Functions
//

static
ISTATUS
SobolImageSamplerDuplicate(
    _In_opt_ const void *context,
    _Out_ PIMAGE_SAMPLER *duplicate
    )
{
    PCSOBOL_IMAGE_SAMPLER sobol_image_sampler = (PCSOBOL_IMAGE_SAMPLER)context;

    ISTATUS status = ImageSamplerAllocate(&sobol_image_sampler_vtable,
                                          sobol_image_sampler,
                                          sizeof(SOBOL_IMAGE_SAMPLER),
                                          alignof(SOBOL_IMAGE_SAMPLER),
                                          duplicate);

    return status;
}

//
// Functions
//

ISTATUS
SobolImageSamplerAllocate(
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

    SOBOL_IMAGE_SAMPLER sobol_image_sampler;
    sobol_image_sampler.samples_per_pixel = samples_per_pixel;

    ISTATUS status = ImageSamplerAllocate(&sobol_image_sampler_vtable,
                                          &sobol_image_sampler,
                                          sizeof(SOBOL_IMAGE_SAMPLER),
                                          alignof(SOBOL_IMAGE_SAMPLER),
                                          image_sampler);

    return status;
}

static_assert(SOBOL_DOUBLE_SIZE == SOBOL_MATRIX_SIZE,
              "pbrt and gruenschloss sobol matrices must be the same size");