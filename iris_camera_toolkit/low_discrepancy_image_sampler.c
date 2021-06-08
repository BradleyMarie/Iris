/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    low_discrepancy_image_sampler.c

Abstract:

    Implements a low_discrepancy image sampler.

--*/

#include <float.h>
#include <stdalign.h>
#include <stdlib.h>

#include "iris_camera_toolkit/low_discrepancy_image_sampler.h"

//
// RNG Types
//

typedef struct _LOW_DISCREPANCY_RANDOM {
    PLOW_DISCREPANCY_SEQUENCE sequence;
    bool owned;
} LOW_DISCREPANCY_RANDOM, *PLOW_DISCREPANCY_RANDOM;

//
// RNG Static Functions
//

static
ISTATUS
LowDiscrepancyRandomGenerateFloat(
    _In_ void *context,
    _In_ float_t minimum,
    _In_ float_t maximum,
    _Out_range_(minimum, maximum) float_t *result
    )
{
    PLOW_DISCREPANCY_RANDOM random = (PLOW_DISCREPANCY_RANDOM)context;

    float_t value;
    ISTATUS status = LowDiscrepancySequenceNextFloat(random->sequence,
                                                     &value);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    value *= (maximum - minimum);
    value += minimum;

    *result = value;

    return ISTATUS_SUCCESS;
}

static
ISTATUS
LowDiscrepancyRandomGenerateIndex(
    _In_ void *context,
    _In_ size_t upper_bound,
    _Out_range_(0, upper_bound - 1) size_t *result
    )
{
    PLOW_DISCREPANCY_RANDOM random = (PLOW_DISCREPANCY_RANDOM)context;

#if FLT_EVAL_METHOD == 0
    if (upper_bound >= (1 << 23))
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }
#else
    if (upper_bound >= (1 << 52))
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }
#endif

    float_t value;
    ISTATUS status = LowDiscrepancySequenceNextFloat(random->sequence,
                                                     &value);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    *result = value * (float_t)upper_bound;
    if (*result == upper_bound)
    {
        *result -= 1;
    }

    return ISTATUS_SUCCESS;
}

static
ISTATUS
LowDiscrepancyRandomReplicate(
    _In_opt_ void *context,
    _Out_ PRANDOM *replica
    );

static
void
LowDiscrepancyRandomFree(
    _In_opt_ _Post_invalid_ void *context
    )
{
    PLOW_DISCREPANCY_RANDOM random = (PLOW_DISCREPANCY_RANDOM)context;

    if (random->owned)
    {
        LowDiscrepancySequenceFree(random->sequence);
    }
}

//
// RNG Static Variables
//

static const RANDOM_VTABLE low_discrepancy_vtable = {
    LowDiscrepancyRandomGenerateFloat,
    LowDiscrepancyRandomGenerateIndex,
    LowDiscrepancyRandomReplicate,
    LowDiscrepancyRandomFree
};

//
// RNG Static Functions
//

static
ISTATUS
LowDiscrepancyRandomReplicate(
    _In_opt_ void *context,
    _Out_ PRANDOM *replica
    )
{
    PLOW_DISCREPANCY_RANDOM random = (PLOW_DISCREPANCY_RANDOM)context;

    LOW_DISCREPANCY_RANDOM next;
    ISTATUS status = LowDiscrepancySequenceDuplicate(random->sequence,
                                                     &next.sequence);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    next.owned = true;

    status = RandomAllocate(&low_discrepancy_vtable,
                            &next,
                            sizeof(LOW_DISCREPANCY_RANDOM),
                            alignof(LOW_DISCREPANCY_RANDOM),
                            replica);

    return status;
}

static
ISTATUS
LowDiscrepancyRandomAllocate(
    _In_ PLOW_DISCREPANCY_SEQUENCE sequence,
    _Out_ PRANDOM *rng
    )
{
    assert(sequence != NULL);
    assert(rng != NULL);

    LOW_DISCREPANCY_RANDOM low_discrepancy;
    low_discrepancy.sequence = sequence;
    low_discrepancy.owned = false;

    ISTATUS status = RandomAllocate(&low_discrepancy_vtable,
                                    &low_discrepancy,
                                    sizeof(LOW_DISCREPANCY_RANDOM),
                                    alignof(LOW_DISCREPANCY_RANDOM),
                                    rng);

    return status;
}

//
// Types
//

typedef struct _LOW_DISCREPANCY_IMAGE_SAMPLER {
    PLOW_DISCREPANCY_SEQUENCE sequence;
    uint32_t samples_per_pixel;
    uint32_t sample_index;
    size_t column;
    size_t num_columns;
    size_t row;
    size_t num_rows;
    float_t dpixel_u;
    float_t dpixel_v;
} LOW_DISCREPANCY_IMAGE_SAMPLER, *PLOW_DISCREPANCY_IMAGE_SAMPLER;

typedef const LOW_DISCREPANCY_IMAGE_SAMPLER *PCLOW_DISCREPANCY_IMAGE_SAMPLER;

//
// Static Functions
//

static
ISTATUS
LowDiscrepancyImageSamplerRandom(
    _In_ void *context,
    _Out_ PRANDOM *rng
    )
{
    PLOW_DISCREPANCY_IMAGE_SAMPLER image_sampler = (PLOW_DISCREPANCY_IMAGE_SAMPLER)context;

    ISTATUS status = LowDiscrepancyRandomAllocate(image_sampler->sequence,
                                                  rng);

    return status;
}

static
ISTATUS
LowDiscrepancyImageSamplerStart(
    _In_ void *context,
    _In_ size_t column,
    _In_ size_t num_columns,
    _In_ size_t row,
    _In_ size_t num_rows,
    _Out_ uint32_t *num_samples
    )
{
    PLOW_DISCREPANCY_IMAGE_SAMPLER image_sampler = (PLOW_DISCREPANCY_IMAGE_SAMPLER)context;

    image_sampler->column = column;
    image_sampler->num_columns = num_columns;
    image_sampler->row = row;
    image_sampler->num_rows = num_rows;
    image_sampler->sample_index = 0;

    double_t sqrt_samples = sqrt((double_t)image_sampler->samples_per_pixel);
    image_sampler->dpixel_u =
        (float_t)((double_t)1.0 / ((double_t)num_columns * sqrt_samples));
    image_sampler->dpixel_v =
        (float_t)((double_t)1.0 / ((double_t)num_rows * sqrt_samples));

    *num_samples = image_sampler->samples_per_pixel;

    return ISTATUS_SUCCESS;
}

static
ISTATUS
LowDiscrepancyImageSamplerNext(
    _In_ void *context,
    _Inout_ PRANDOM pixel_rng,
    _Out_ float_t *pixel_u,
    _Out_ float_t *pixel_v,
    _Out_ float_t *dpixel_u,
    _Out_ float_t *dpixel_v,
    _Out_opt_ float_t *lens_u,
    _Out_opt_ float_t *lens_v
    )
{
    PLOW_DISCREPANCY_IMAGE_SAMPLER image_sampler = (PLOW_DISCREPANCY_IMAGE_SAMPLER)context;

    uint64_t sequence_index;
    ISTATUS status = LowDiscrepancySequenceComputeIndex(image_sampler->sequence,
                                                        image_sampler->column,
                                                        image_sampler->num_columns,
                                                        image_sampler->row,
                                                        image_sampler->num_rows,
                                                        image_sampler->sample_index,
                                                        image_sampler->samples_per_pixel,
                                                        &sequence_index);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    status = LowDiscrepancySequenceStart(image_sampler->sequence,
                                         sequence_index);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    double_t pixel_u_as_double;
    status = LowDiscrepancySequenceNextDouble(image_sampler->sequence,
                                              &pixel_u_as_double);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    *pixel_u = (float_t)pixel_u_as_double;

    double_t pixel_v_as_double;
    status = LowDiscrepancySequenceNextDouble(image_sampler->sequence,
                                              &pixel_v_as_double);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    *pixel_v = (float_t)pixel_v_as_double;

    *dpixel_u = image_sampler->dpixel_u;
    *dpixel_v = image_sampler->dpixel_v;

    if (lens_u != NULL)
    {
        status = LowDiscrepancySequenceNextFloat(image_sampler->sequence,
                                                 lens_u);

        if (status != ISTATUS_SUCCESS)
        {
            return status;
        }
    }

    if (lens_v != NULL)
    {
        status = LowDiscrepancySequenceNextFloat(image_sampler->sequence,
                                                 lens_v);

        if (status != ISTATUS_SUCCESS)
        {
            return status;
        }
    }

    image_sampler->sample_index += 1;

    return ISTATUS_SUCCESS;
}

static
ISTATUS
LowDiscrepancyImageSamplerDuplicate(
    _In_opt_ const void *context,
    _Out_ PIMAGE_SAMPLER *replica
    )
{
    PCLOW_DISCREPANCY_IMAGE_SAMPLER image_sampler = (PCLOW_DISCREPANCY_IMAGE_SAMPLER)context;

    PLOW_DISCREPANCY_SEQUENCE sequence;
    ISTATUS status = LowDiscrepancySequenceDuplicate(image_sampler->sequence,
                                                     &sequence);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    status = LowDiscrepancyImageSamplerAllocate(sequence,
                                                image_sampler->samples_per_pixel,
                                                replica);

    if (status != ISTATUS_SUCCESS)
    {
        LowDiscrepancySequenceFree(sequence);
        return status;
    }

    return ISTATUS_SUCCESS;
}

static
void
LowDiscrepancyImageSamplerFree(
    _In_opt_ _Post_invalid_ void *context
    )
{
    PLOW_DISCREPANCY_IMAGE_SAMPLER image_sampler = (PLOW_DISCREPANCY_IMAGE_SAMPLER)context;

    LowDiscrepancySequenceFree(image_sampler->sequence);
}

//
// Static Variables
//

static const IMAGE_SAMPLER_VTABLE low_discrepancy_image_sampler_vtable = {
    NULL,
    LowDiscrepancyImageSamplerRandom,
    LowDiscrepancyImageSamplerStart,
    LowDiscrepancyImageSamplerNext,
    LowDiscrepancyImageSamplerDuplicate,
    LowDiscrepancyImageSamplerFree
};

//
// Functions
//

ISTATUS
LowDiscrepancyImageSamplerAllocate(
    _In_ PCLOW_DISCREPANCY_SEQUENCE sequence,
    _In_ uint32_t samples_per_pixel,
    _Out_ PIMAGE_SAMPLER *image_sampler
    )
{
    if (sequence == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (samples_per_pixel == 0)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (image_sampler == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    LOW_DISCREPANCY_IMAGE_SAMPLER sampler;
    sampler.samples_per_pixel = samples_per_pixel;

    ISTATUS status = LowDiscrepancySequenceDuplicate(sequence,
                                                     &sampler.sequence);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    status = ImageSamplerAllocate(&low_discrepancy_image_sampler_vtable,
                                  &sampler,
                                  sizeof(LOW_DISCREPANCY_IMAGE_SAMPLER),
                                  alignof(LOW_DISCREPANCY_IMAGE_SAMPLER),
                                  image_sampler);

    if (status != ISTATUS_SUCCESS)
    {
        LowDiscrepancySequenceFree(sampler.sequence);
        return status;
    }

    return ISTATUS_SUCCESS;
}