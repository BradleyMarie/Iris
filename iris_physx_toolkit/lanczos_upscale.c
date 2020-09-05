/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    lanczos_upscale.c

Abstract:

    Upscale an image to be the next power of two in each dimension using
    Lanczos resampling.

--*/

#include "iris_physx_toolkit/lanczos_upscale.h"

#include "limits.h"
#include "stdlib.h"

#include "common/safe_math.h"

//
// Static Data
//

static const float_t filter_width = (float_t)2.0;
static const float_t tau = (float_t)2.0;

//
// Static Functions
//

float_t
Lanczos(
    _In_ float_t x
    )
{
    x = fabs(x);

    if (x < (float_t)1e-5)
    {
        return 1;
    }

    if (x > (float_t)1.0)
    {
        return 0;
    }

    x *= iris_pi;
    float_t s = sin(x * tau) / (x * tau);
    float_t lanczos = sin(x) / x;
    
    return s * lanczos;
}

static
inline
size_t
RoundUpToPowerOfTwo(
    _In_ size_t value
    )
{
    value -= 1;
    value |= value >> 1;
    value |= value >> 2;
    value |= value >> 4;
    value |= value >> 8;
    value |= value >> 16;

#if SIZE_MAX == UINT64_MAX

    value |= value >> 32;

#endif // SIZE_MAX == UINT64_MAX

    value += 1;

    return value;
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

void
ComputeWeights(
    _In_ size_t old_resolution,
    _In_ size_t new_resolution,
    _In_ size_t output_index,
    _Out_ size_t *input_start_index,
    _Out_ float_t weights[4],
    _Out_ size_t *valid_weights
    )
{
    assert(old_resolution != 0);
    assert(new_resolution != 0 && (new_resolution & (new_resolution - 1)) == 0);
    assert(output_index < new_resolution);
    assert(input_start_index != NULL);
    assert(weights != NULL);
    assert(valid_weights != NULL);

    if (old_resolution == new_resolution)
    {
        *input_start_index = output_index;
        weights[0] = (float_t)1.0;
        *valid_weights = 1;
        return;
    }

    float_t center = (float_t)output_index + (float_t)0.5;
    center *= (float_t)old_resolution / (float_t)new_resolution;

    float_t start_index = floor(center - filter_width + (float_t)0.5);

    size_t num_weights = 4;
    for (size_t i = 0; start_index < (float_t)0.0 && num_weights != 0; i++)
    {
        start_index += (float_t)1.0;
    }

    for (size_t i = 0;
         (float_t)old_resolution <= start_index && num_weights != 0;
         i++) {
        start_index -= (float_t)1.0;
    }

    assert(num_weights != 0);

    *input_start_index = (size_t)start_index;

    float_t sum_weights = (float_t)0.0;
    for (size_t i = 0; i < num_weights; i++)
    {
        float_t position = start_index + (float_t)i + (float_t)0.5;
        weights[i] = Lanczos((position - center) / filter_width);
        sum_weights += weights[i];
    }

    float_t inv_sum_weights = (float_t)1.0 / sum_weights;

    for (size_t i = 0; i < num_weights; i++)
    {
        weights[i] *= inv_sum_weights;
    }
}

//
// Functions
//

ISTATUS
LancsozUpscaleBytes(
    _In_reads_(height * width) _Post_invalid_ unsigned char texels[],
    _In_ size_t width,
    _In_ size_t height,
    _Outptr_result_buffer_(*new_width * *new_height) unsigned char **new_texels,
    _Out_ size_t* new_width,
    _Out_ size_t* new_height
    )
{
    if (texels == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (width == 0)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (height == 0)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (new_texels == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (new_width == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    if (new_height == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }

    *new_width = RoundUpToPowerOfTwo(width);

    if (*new_width == 0)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    *new_height = RoundUpToPowerOfTwo(height);

    if (*new_height == 0)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    if (width == *new_width && height == *new_height)
    {
        *new_texels = texels;
        return ISTATUS_SUCCESS;
    }

    size_t num_pixels;
    bool success = CheckedMultiplySizeT(*new_width,
                                        *new_height,
                                        &num_pixels);

    if (!success)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    unsigned char *new_texels_buffer =
        (unsigned char *)calloc(num_pixels, sizeof(unsigned char));

    if (new_texels_buffer == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    for (size_t i = 0; i < height; i++)
    {
        for (size_t j = 0; j < *new_width; j++)
        {
            size_t start_index;
            float_t weights[4];
            size_t valid_weights;
            ComputeWeights(width,
                           *new_width,
                           j,
                           &start_index,
                           weights,
                           &valid_weights);

            float_t new_value = (float_t)0.0;
            for (size_t k = 0; k < valid_weights; k++)
            {
                new_value +=
                    (float_t)texels[i * height + start_index + k] * weights[k];
            }

            new_value = fmax((float_t)0.0, new_value);
            new_value = fmin((float_t)UCHAR_MAX, new_value);
            new_value = round(new_value);

            new_texels_buffer[i * *new_width + j] = (unsigned char)new_value;
        }
    }

    for (size_t i = 0; i < *new_width; i++)
    {
        for (size_t j = 0; j < *new_height; j++)
        {
            size_t start_index;
            float_t weights[4];
            size_t valid_weights;
            ComputeWeights(height,
                           *new_height,
                           j,
                           &start_index,
                           weights,
                           &valid_weights);

            float_t new_value = (float_t)0.0;
            for (size_t k = 0; k < valid_weights; k++)
            {
                new_value +=
                    (float_t)new_texels_buffer[j * *new_height + start_index + k] * weights[k];
            }

            new_value = fmax((float_t)0.0, new_value);
            new_value = fmin((float_t)UCHAR_MAX, new_value);
            new_value = round(new_value);

            new_texels_buffer[j * *new_width + i] = (unsigned char)new_value;
        }
    }

    *new_texels = new_texels_buffer;

    free(texels);

    return ISTATUS_SUCCESS;
}

ISTATUS
LanczosUpscaleByteTuples(
    _In_reads_(height * width) _Post_invalid_ unsigned char texels[][3],
    _In_ size_t width,
    _In_ size_t height,
    _Outptr_result_buffer_(*new_width * *new_height) unsigned char (**new_texels)[3],
    _Out_ size_t* new_width,
    _Out_ size_t* new_height
    )
{
    if (texels == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (width == 0)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (height == 0)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (new_texels == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (new_width == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    if (new_height == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }

    *new_width = RoundUpToPowerOfTwo(width);

    if (*new_width == 0)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    *new_height = RoundUpToPowerOfTwo(height);

    if (*new_height == 0)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    if (width == *new_width && height == *new_height)
    {
        *new_texels = texels;
        return ISTATUS_SUCCESS;
    }

    size_t num_pixels;
    bool success = CheckedMultiplySizeT(*new_width,
                                        *new_height,
                                        &num_pixels);

    if (!success)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    unsigned char (*new_texels_buffer)[3] =
        (unsigned char (*)[3])calloc(num_pixels, sizeof(unsigned char) * 3);

    if (new_texels_buffer == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    for (size_t i = 0; i < height; i++)
    {
        for (size_t j = 0; j < *new_width; j++)
        {
            size_t start_index;
            float_t weights[4];
            size_t valid_weights;
            ComputeWeights(width,
                           *new_width,
                           j,
                           &start_index,
                           weights,
                           &valid_weights);

            float_t new_values[3];
            new_values[0] = new_values[1] = new_values[2] = (float_t)0.0;
            for (size_t k = 0; k < valid_weights; k++)
            {
                new_values[0] +=
                    (float_t)texels[i * height + start_index + k][0] * weights[k];
                new_values[1] +=
                    (float_t)texels[i * height + start_index + k][1] * weights[k];
                new_values[2] +=
                    (float_t)texels[i * height + start_index + k][2] * weights[k];
            }

            new_values[0] = fmax((float_t)0.0, new_values[0]);
            new_values[0] = fmin((float_t)UCHAR_MAX, new_values[0]);
            new_values[0] = round(new_values[0]);

            new_values[1] = fmax((float_t)0.0, new_values[1]);
            new_values[1] = fmin((float_t)UCHAR_MAX, new_values[1]);
            new_values[1] = round(new_values[1]);

            new_values[2] = fmax((float_t)0.0, new_values[2]);
            new_values[2] = fmin((float_t)UCHAR_MAX, new_values[2]);
            new_values[2] = round(new_values[2]);

            new_texels_buffer[i * *new_width + j][0] = (unsigned char)new_values[0];
            new_texels_buffer[i * *new_width + j][1] = (unsigned char)new_values[1];
            new_texels_buffer[i * *new_width + j][2] = (unsigned char)new_values[2];
        }
    }

    for (size_t i = 0; i < *new_width; i++)
    {
        for (size_t j = 0; j < *new_height; j++)
        {
            size_t start_index;
            float_t weights[4];
            size_t valid_weights;
            ComputeWeights(height,
                           *new_height,
                           j,
                           &start_index,
                           weights,
                           &valid_weights);

            float_t new_values[3];
            new_values[0] = new_values[1] = new_values[2] = (float_t)0.0;
            for (size_t k = 0; k < valid_weights; k++)
            {
                new_values[0] +=
                    (float_t)new_texels_buffer[j * *new_height + start_index + k][0] * weights[k];
                new_values[1] +=
                    (float_t)new_texels_buffer[j * *new_height + start_index + k][1] * weights[k];
                new_values[2] +=
                    (float_t)new_texels_buffer[j * *new_height + start_index + k][2] * weights[k];
            }

            new_values[0] = fmax((float_t)0.0, new_values[0]);
            new_values[0] = fmin((float_t)UCHAR_MAX, new_values[0]);
            new_values[0] = round(new_values[0]);

            new_values[1] = fmax((float_t)0.0, new_values[1]);
            new_values[1] = fmin((float_t)UCHAR_MAX, new_values[1]);
            new_values[1] = round(new_values[1]);

            new_values[2] = fmax((float_t)0.0, new_values[2]);
            new_values[2] = fmin((float_t)UCHAR_MAX, new_values[2]);
            new_values[2] = round(new_values[2]);

            new_texels_buffer[j * *new_width + i][0] = (unsigned char)new_values[0];
            new_texels_buffer[j * *new_width + i][1] = (unsigned char)new_values[1];
            new_texels_buffer[j * *new_width + i][2] = (unsigned char)new_values[2];
        }
    }

    *new_texels = new_texels_buffer;

    free(texels);

    return ISTATUS_SUCCESS;
}

ISTATUS
LanczosUpscaleFloatTuples(
    _In_reads_(height * width) _Post_invalid_ float_t texels[][3],
    _In_ size_t width,
    _In_ size_t height,
    _Outptr_result_buffer_(*new_width * *new_height) float_t (**new_texels)[3],
    _Out_ size_t* new_width,
    _Out_ size_t* new_height
    )
{
    if (texels == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (width == 0)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (height == 0)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (new_texels == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (new_width == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    if (new_height == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }

    *new_width = RoundUpToPowerOfTwo(width);

    if (*new_width == 0)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    *new_height = RoundUpToPowerOfTwo(height);

    if (*new_height == 0)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    if (width == *new_width && height == *new_height)
    {
        *new_texels = texels;
        return ISTATUS_SUCCESS;
    }

    size_t num_pixels;
    bool success = CheckedMultiplySizeT(*new_width,
                                        *new_height,
                                        &num_pixels);

    if (!success)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    float_t (*new_texels_buffer)[3] =
        (float_t (*)[3])calloc(num_pixels, sizeof(float_t) * 3);

    if (new_texels_buffer == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    for (size_t i = 0; i < height; i++)
    {
        for (size_t j = 0; j < *new_width; j++)
        {
            size_t start_index;
            float_t weights[4];
            size_t valid_weights;
            ComputeWeights(width,
                           *new_width,
                           j,
                           &start_index,
                           weights,
                           &valid_weights);

            float_t new_values[3];
            new_values[0] = new_values[1] = new_values[2] = (float_t)0.0;
            for (size_t k = 0; k < valid_weights; k++)
            {
                new_values[0] +=
                    (float_t)texels[i * height + start_index + k][0] * weights[k];
                new_values[1] +=
                    (float_t)texels[i * height + start_index + k][1] * weights[k];
                new_values[2] +=
                    (float_t)texels[i * height + start_index + k][2] * weights[k];
            }

            new_texels_buffer[i * *new_width + j][0] = new_values[0];
            new_texels_buffer[i * *new_width + j][1] = new_values[1];
            new_texels_buffer[i * *new_width + j][2] = new_values[2];
        }
    }

    for (size_t i = 0; i < *new_width; i++)
    {
        for (size_t j = 0; j < *new_height; j++)
        {
            size_t start_index;
            float_t weights[4];
            size_t valid_weights;
            ComputeWeights(height,
                           *new_height,
                           j,
                           &start_index,
                           weights,
                           &valid_weights);

            float_t new_values[3];
            new_values[0] = new_values[1] = new_values[2] = (float_t)0.0;
            for (size_t k = 0; k < valid_weights; k++)
            {
                new_values[0] +=
                    (float_t)new_texels_buffer[j * *new_height + start_index + k][0] * weights[k];
                new_values[1] +=
                    (float_t)new_texels_buffer[j * *new_height + start_index + k][1] * weights[k];
                new_values[2] +=
                    (float_t)new_texels_buffer[j * *new_height + start_index + k][2] * weights[k];
            }

            new_texels_buffer[j * *new_width + i][0] = new_values[0];
            new_texels_buffer[j * *new_width + i][1] = new_values[1];
            new_texels_buffer[j * *new_width + i][2] = new_values[2];
        }
    }

    *new_texels = new_texels_buffer;

    free(texels);

    return ISTATUS_SUCCESS;
}