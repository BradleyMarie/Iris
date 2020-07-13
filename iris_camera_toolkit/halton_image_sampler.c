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
#include "third_party/gruenschloss/halton/halton_enum.h"

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
    Halton_enum halton_enum;
    unsigned current_sample_index;
    uint32_t samples_per_pixel;
    double_t to_pixel_u;
    double_t to_pixel_v;
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
        int_to_double.as_int |= 0x3FF0000000000000;
        int_to_double.as_double -= 1.0;

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
ISTATUS
HaltonImageSamplerPrepareImageSamples(
    _In_ void *context,
    _Inout_ PRANDOM rng,
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
    _Inout_ PRANDOM rng,
    _In_ size_t sample_index,
    _Out_ float_t *pixel_sample_u,
    _Out_ float_t *pixel_sample_v,
    _Out_ float_t *lens_sample_u,
    _Out_ float_t *lens_sample_v
    )
{
    PHALTON_IMAGE_SAMPLER image_sampler = (PHALTON_IMAGE_SAMPLER)context;

    unsigned index =
        image_sampler->current_sample_index +
        image_sampler->halton_enum.m_increment * (unsigned)sample_index;

    double_t halton_u = HaltonSequenceCompute((uint64_t)index, COLUMN_BASE);
    *pixel_sample_u = (float_t)(halton_u * image_sampler->to_pixel_u);

    double_t halton_v = HaltonSequenceCompute((uint64_t)index, ROW_BASE);
    *pixel_sample_v = (float_t)(halton_v * image_sampler->to_pixel_v);

    if (image_sampler->lens_delta_u != (float_t)0.0)
    {
        float_t value =
            (float_t)HaltonSequenceCompute((uint64_t)index, LENS_U_BASE);
        *lens_sample_u =
             image_sampler->lens_min_u + image_sampler->lens_delta_u * value;
    }
    else
    {
        *lens_sample_u = image_sampler->lens_min_u;
    }

    if (image_sampler->lens_delta_v != (float_t)0.0)
    {
        double_t value =
            (float_t)HaltonSequenceCompute((uint64_t)index, LENS_V_BASE);
        *lens_sample_v =
            image_sampler->lens_min_v + image_sampler->lens_delta_v * value;
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

    ISTATUS status = ImageSamplerAllocate(&halton_image_sampler_vtable,
                                          &halton_image_sampler,
                                          sizeof(HALTON_IMAGE_SAMPLER),
                                          alignof(HALTON_IMAGE_SAMPLER),
                                          image_sampler);

    return status;
}