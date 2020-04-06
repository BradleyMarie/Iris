/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    halton_image_sampler.c

Abstract:

    Implements a halton image sampler.

--*/

#include <float.h>
#include <stdalign.h>

#include "common/safe_math.h"
#include "iris_camera_toolkit/halton_image_sampler.h"

//
// Defines
//

#define COLUMN_BASE 2
#define ROW_BASE    3
#define LENS_U_BASE 5
#define LENS_V_BASE 7

#if FLT_EVAL_METHOD	== 0
const static double one_minus_epsilon = 1.0 - DBL_EPSILON;
#elif FLT_EVAL_METHOD == 1
const static double one_minus_epsilon = 1.0 - DBL_EPSILON;
#elif FLT_EVAL_METHOD == 2
const static long double one_minus_epsilon = 1.0L - LDBL_EPSILON;
#endif

//
// Types
//

typedef struct _HALTON_IMAGE_SAMPLER {
    uint16_t samples_per_pixel;
    uint64_t sample_space_num_columns;
    uint64_t sample_space_num_columns_num_digits;
    uint64_t sample_space_column_multiplicative_inverse;
    uint64_t sample_space_num_rows;
    uint64_t sample_space_num_rows_num_digits;
    uint64_t sample_space_row_multiplicative_inverse;
    uint64_t pixel_sample_period;
    uint64_t current_sample_index;
    float_t lens_min_u;
    float_t lens_delta_u;
    float_t lens_min_v;
    float_t lens_delta_v;
} HALTON_IMAGE_SAMPLER, *PHALTON_IMAGE_SAMPLER;

typedef const HALTON_IMAGE_SAMPLER *PCHALTON_IMAGE_SAMPLER;

//
// Static Functions
//


static
void
ExtendedGcd(
    _In_ uint64_t a,
    _In_ uint64_t b,
    _In_ uint64_t *x,
    _In_ uint64_t *y
    )
{
    if (b == 0)
    {
        *x = 1;
        *y = 0;
        return;
    }

    uint64_t xp, yp;
    ExtendedGcd(b, a % b, &xp, &yp);

    uint64_t d = a / b;
    *x = yp;
    *y = xp - (d * yp);
}

static
uint64_t
MultiplicativeInverse(
    _In_ uint64_t a,
    _In_ uint64_t n
    )
{
    uint64_t x, y;
    ExtendedGcd(a, n, &x, &y);
    return x % n;
}

static
inline
uint32_t
ReverseBits32(
    _In_ uint32_t value
    )
{
    value = (value << 16) | (value >> 16);
    value = ((value & 0x00ff00ff) << 8) | ((value & 0xff00ff00) >> 8);
    value = ((value & 0x0f0f0f0f) << 4) | ((value & 0xf0f0f0f0) >> 4);
    value = ((value & 0x33333333) << 2) | ((value & 0xcccccccc) >> 2);
    value = ((value & 0x55555555) << 1) | ((value & 0xaaaaaaaa) >> 1);

    return value;
}

static
inline
uint64_t
ReverseBits64(
    _In_ uint64_t value
    )
{
    uint64_t high = ReverseBits32((uint32_t)value);
    uint64_t low = ReverseBits32((uint32_t)(value >> 32));
    uint64_t result = (high << 32) | low;

    return result;
}

static
inline
double_t
HaltonSequenceCompute(
    _In_ uint64_t index,
    _In_ uint64_t base
    )
{
    assert(base == 2 || base == 3 || base == 5 || base == 7);

    if (base == 2)
    {
        union {
            uint64_t as_int;
            double as_double;
        } int_to_double;

        int_to_double.as_int = ReverseBits64(index) >> 12;
        int_to_double.as_int |= 0x3FC0000000000000;

        return (double_t)int_to_double.as_double;
    }

    double_t inverse_base = 1.0 / (double_t)base;
    uint64_t reversed_numerals = 0;
    double_t inverse_base_product = 1.0;

    while (index != 0)
    {
        uint64_t next = index / base;
        uint64_t numeral = index - (next * base);
        reversed_numerals = (reversed_numerals * base) + numeral;
        inverse_base_product *= inverse_base;
        index = next;
    }

    double_t result = (double_t)reversed_numerals * inverse_base_product;

    return fmin(result, one_minus_epsilon);
}

static
inline
uint64_t
InverseRadicalInverse(
    _In_ uint64_t radical_inverse,
    _In_ uint64_t base,
    _In_ uint64_t num_digits
    )
{
    uint64_t index = 0;
    for (int i = 0; i < num_digits; i++)
    {
        uint64_t numeral = radical_inverse % base;
        radical_inverse /= base;
        index = index * base + numeral;
    }

    return index;
}

static
ISTATUS
HaltonComputeSampleSpaceDimensionSize(
    _In_ uint64_t framebuffer_dimension_size,
    _In_ uint64_t base,
    _Out_ uint64_t *sample_space_dimension_size,
    _Out_ uint64_t *sample_space_dimension_size_num_digits
    )
{
    *sample_space_dimension_size = 1;
    *sample_space_dimension_size_num_digits = 0;

    while (*sample_space_dimension_size < framebuffer_dimension_size)
    {
        bool success = CheckedMultiplySizeT(*sample_space_dimension_size,
                                            base,
                                            sample_space_dimension_size);

        if (!success)
        {
            return ISTATUS_INTEGER_OVERFLOW;
        }

        *sample_space_dimension_size_num_digits += 1;
    }

    return ISTATUS_SUCCESS;
}

static
ISTATUS
HaltonImageSamplerPrepareImageSamples(
    _In_ void *context,
    _Inout_ PRANDOM rng,
    _In_ size_t num_columns,
    _In_ size_t num_rows
    )
{
    PHALTON_IMAGE_SAMPLER image_sampler = (PHALTON_IMAGE_SAMPLER)context;

    ISTATUS status =
        HaltonComputeSampleSpaceDimensionSize(num_columns,
                                              COLUMN_BASE,
                                              &image_sampler->sample_space_num_columns,
                                              &image_sampler->sample_space_num_columns_num_digits);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    status =
        HaltonComputeSampleSpaceDimensionSize(num_rows,
                                              ROW_BASE,
                                              &image_sampler->sample_space_num_rows,
                                              &image_sampler->sample_space_num_rows_num_digits);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    bool success =
        CheckedMultiplySizeT(image_sampler->sample_space_num_columns,
                             image_sampler->sample_space_num_rows,
                             &image_sampler->pixel_sample_period);

    if (!success)
    {
        return ISTATUS_INTEGER_OVERFLOW;
    }

    size_t unused;
    success = CheckedMultiplySizeT(image_sampler->pixel_sample_period,
                                   image_sampler->samples_per_pixel,
                                   &unused);

    if (!success)
    {
        return ISTATUS_INTEGER_OVERFLOW;
    }

    image_sampler->sample_space_column_multiplicative_inverse =
        MultiplicativeInverse(image_sampler->sample_space_num_rows,
                              image_sampler->sample_space_num_columns);

    image_sampler->sample_space_row_multiplicative_inverse =
        MultiplicativeInverse(image_sampler->sample_space_num_columns,
                              image_sampler->sample_space_num_rows);

    return ISTATUS_SUCCESS;
}

static
ISTATUS
HaltonImageSamplerPreparePixelSamples(
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
    PHALTON_IMAGE_SAMPLER image_sampler = (PHALTON_IMAGE_SAMPLER)context;

    uint64_t column_offset = InverseRadicalInverse(column,
                                                   COLUMN_BASE,
                                                   image_sampler->sample_space_num_columns_num_digits);

    image_sampler->current_sample_index =
        column_offset *
        (image_sampler->pixel_sample_period / image_sampler->sample_space_num_columns) *
        image_sampler->sample_space_column_multiplicative_inverse;

    uint64_t row_offset = InverseRadicalInverse(row,
                                                ROW_BASE,
                                                image_sampler->sample_space_num_rows_num_digits);

    image_sampler->current_sample_index +=
        row_offset *
        (image_sampler->pixel_sample_period / image_sampler->sample_space_num_rows) *
        image_sampler->sample_space_row_multiplicative_inverse;

    image_sampler->current_sample_index %= image_sampler->pixel_sample_period;

    image_sampler->lens_min_u = lens_min_u;
    image_sampler->lens_min_v = lens_min_v;
    image_sampler->lens_delta_u = lens_max_u - lens_min_u;
    image_sampler->lens_delta_v = lens_max_v - lens_min_v;

    *num_samples = image_sampler->samples_per_pixel;

    return ISTATUS_SUCCESS;
}

static
ISTATUS
HaltonImageSamplerNextSample(
    _In_ const void *context,
    _Inout_ PRANDOM rng,
    _In_ size_t sample_index,
    _Out_ float_t *pixel_sample_u,
    _Out_ float_t *pixel_sample_v,
    _Out_ float_t *lens_sample_u,
    _Out_ float_t *lens_sample_v
    )
{
    PHALTON_IMAGE_SAMPLER image_sampler = (PHALTON_IMAGE_SAMPLER)context;

    uint64_t halton_offset = image_sampler->pixel_sample_period * sample_index;
    uint64_t halton_index = image_sampler->current_sample_index + halton_offset;

    *pixel_sample_u = (float_t)HaltonSequenceCompute(halton_index, COLUMN_BASE);
    *pixel_sample_v = (float_t)HaltonSequenceCompute(halton_index, ROW_BASE);

    if (image_sampler->lens_delta_u != (float_t)0.0)
    {
        float_t value =
            (float_t)HaltonSequenceCompute(halton_index, LENS_U_BASE);
        *lens_sample_u = fma(value,
                             image_sampler->lens_delta_u,
                             image_sampler->lens_min_u);
    }
    else
    {
        *lens_sample_u = image_sampler->lens_min_u;
    }

    if (image_sampler->lens_delta_v != (float_t)0.0)
    {
        double_t value =
            (float_t)HaltonSequenceCompute(halton_index, LENS_V_BASE);
        *lens_sample_v = fma(value,
                             image_sampler->lens_delta_v,
                             image_sampler->lens_min_v);
    }
    else
    {
        *lens_sample_v = image_sampler->lens_min_v;
    }

    return ISTATUS_SUCCESS;
}

static
ISTATUS
HaltonImageSamplerDuplicate(
    _In_opt_ const void *context,
    _Out_ PIMAGE_SAMPLER *duplicate
    );

//
// Static Variables
//

static const IMAGE_SAMPLER_VTABLE halton_image_sampler_vtable = {
    HaltonImageSamplerPrepareImageSamples,
    HaltonImageSamplerPreparePixelSamples,
    HaltonImageSamplerNextSample,
    HaltonImageSamplerDuplicate,
    NULL
};

//
// Static Functions
//

static
ISTATUS
HaltonImageSamplerDuplicate(
    _In_opt_ const void *context,
    _Out_ PIMAGE_SAMPLER *duplicate
    )
{
    PCHALTON_IMAGE_SAMPLER halton_image_sampler = (PCHALTON_IMAGE_SAMPLER)context;

    ISTATUS status = ImageSamplerAllocate(&halton_image_sampler_vtable,
                                          halton_image_sampler,
                                          sizeof(HALTON_IMAGE_SAMPLER),
                                          alignof(HALTON_IMAGE_SAMPLER),
                                          duplicate);

    return status;
}

//
// Functions
//

ISTATUS
HaltonImageSamplerAllocate(
    _In_ uint16_t samples_per_pixel,
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

    ISTATUS status = ImageSamplerAllocate(&halton_image_sampler_vtable,
                                          &halton_image_sampler,
                                          sizeof(HALTON_IMAGE_SAMPLER),
                                          alignof(HALTON_IMAGE_SAMPLER),
                                          image_sampler);

    return status;
}