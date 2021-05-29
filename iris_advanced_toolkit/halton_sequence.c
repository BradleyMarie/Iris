/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    halton_sequence.c

Abstract:

    Implements a Halton image sampler.

--*/

#include <stdalign.h>
#include <stdatomic.h>
#include <stdlib.h>

#include "iris_advanced_toolkit/halton_sequence.h"
#include "third_party/gruenschloss/halton/halton_enum.h"
#include "third_party/gruenschloss/halton/halton_sampler.h"

//
// Types
//

typedef struct _SHARED_HALTON_DATA {
    Halton_sampler halton_sampler;
    atomic_uintmax_t reference_count;
} SHARED_HALTON_DATA, *PSHARED_HALTON_DATA;

typedef struct _HALTON_SEQUENCE {
    PSHARED_HALTON_DATA halton_data;
    Halton_enum enumerator;
    unsigned num_columns;
    unsigned num_rows;
    unsigned column;
    unsigned row;
    unsigned base_index;
    unsigned index;
    unsigned dimension;
    float_t scale_factor[2];
} HALTON_SEQUENCE, *PHALTON_SEQUENCE;

//
// Static Functions
//

static
ISTATUS
HaltonSequenceComputeIndex(
    _In_ void *context,
    _In_ size_t column,
    _In_ size_t num_columns,
    _In_ size_t row,
    _In_ size_t num_rows,
    _In_ uint32_t sample,
    _In_ uint32_t num_samples,
    _Out_ uint64_t *index
    )
{
    if (UINT32_MAX < num_columns)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (UINT32_MAX < num_rows)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    PHALTON_SEQUENCE halton_sequence = (PHALTON_SEQUENCE)context;

    bool recompute;
    if (halton_sequence->num_columns != num_columns ||
        halton_sequence->num_rows != num_rows)
    {
        halton_sequence->enumerator = halton_enum(num_columns, num_rows);
        halton_sequence->num_columns = num_columns;
        halton_sequence->num_rows = num_rows;
        halton_sequence->scale_factor[0] =
            halton_sequence->enumerator.m_scale_x / (float_t)num_columns;
        halton_sequence->scale_factor[1] =
            halton_sequence->enumerator.m_scale_y / (float_t)num_rows;
        recompute = true;
    }
    else
    {
        recompute = false;
    }

    if (halton_sequence->column != column ||
        halton_sequence->row != row ||
        recompute)
    {
        halton_sequence->base_index = get_index(&halton_sequence->enumerator,
                                                0,
                                                column,
                                                row);
        halton_sequence->column = column;
        halton_sequence->row = row;

        *index = halton_sequence->base_index;
    }
    else
    {
        *index = halton_sequence->base_index +
                 sample * halton_sequence->enumerator.m_increment;
    }

    return ISTATUS_SUCCESS;
}

static
ISTATUS
HaltonSequenceStart(
    _In_ void *context,
    _In_ uint64_t index
    )
{
    if (UINT32_MAX < index)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    PHALTON_SEQUENCE halton_sequence = (PHALTON_SEQUENCE)context;

    halton_sequence->index = index;
    halton_sequence->dimension = 0;

    return ISTATUS_SUCCESS;
}

static
ISTATUS
HaltonSequenceNextFloat(
    _In_ void *context,
    _Out_ float_t *value
    )
{
    PHALTON_SEQUENCE halton_sequence = (PHALTON_SEQUENCE)context;
 
    float as_float;
    bool success = checked_sample(&halton_sequence->halton_data->halton_sampler,
                                  halton_sequence->dimension,
                                  halton_sequence->index,
                                  &as_float);

    if (!success)
    {
        return ISTATUS_OUT_OF_ENTROPY;
    }

    if (halton_sequence->dimension < 2)
    {
        as_float *= halton_sequence->scale_factor[halton_sequence->dimension];
        as_float = IMin(IMax(as_float, (float_t)0.0), (float_t)1.0);
    }

    halton_sequence->dimension += 1;
    *value = as_float;

    return ISTATUS_SUCCESS;
}

static
ISTATUS
HaltonSequenceNextDouble(
    _In_ void *context,
    _Out_ double_t *value
    )
{
    PHALTON_SEQUENCE halton_sequence = (PHALTON_SEQUENCE)context;
 
    float as_float;
    bool success = checked_sample(&halton_sequence->halton_data->halton_sampler,
                                  halton_sequence->dimension,
                                  halton_sequence->index,
                                  &as_float);

    if (!success)
    {
        return ISTATUS_OUT_OF_ENTROPY;
    }

    if (halton_sequence->dimension < 2)
    {
        as_float *= halton_sequence->scale_factor[halton_sequence->dimension];
        as_float = IMin(IMax(as_float, (float_t)0.0), (float_t)1.0);
    }

    halton_sequence->dimension += 1;
    *value = as_float;

    return ISTATUS_SUCCESS;
}

static
ISTATUS
HaltonSequenceDuplicate(
    _In_ const void *context,
    _Out_ PLOW_DISCREPANCY_SEQUENCE *duplicate
    );

static
void
HaltonSequenceFree(
    _In_opt_ _Post_invalid_ void *context
    )
{
    PHALTON_SEQUENCE sequence = (PHALTON_SEQUENCE)context;

    if (atomic_fetch_sub(&sequence->halton_data->reference_count, 1) == 1)
    {
        free(sequence->halton_data);
    }
}

//
// Static Data
//

static const LOW_DISCREPANCY_SEQUENCE_VTABLE halton_sequence_vtable = {
    NULL,
    HaltonSequenceComputeIndex,
    HaltonSequenceStart,
    HaltonSequenceNextFloat,
    HaltonSequenceNextDouble,
    HaltonSequenceDuplicate,
    HaltonSequenceFree
};

//
// Static Functions
//

static
ISTATUS
HaltonSequenceDuplicate(
    _In_ const void *context,
    _Out_ PLOW_DISCREPANCY_SEQUENCE *duplicate
    )
{
    PHALTON_SEQUENCE halton_sequence = (PHALTON_SEQUENCE)context;

    ISTATUS status = LowDiscrepancySequenceAllocate(&halton_sequence_vtable,
                                                    context,
                                                    sizeof(HALTON_SEQUENCE),
                                                    alignof(HALTON_SEQUENCE),
                                                    duplicate);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    atomic_fetch_add(&halton_sequence->halton_data->reference_count, 1);

    return ISTATUS_SUCCESS;
}

//
// Functions
//

ISTATUS
HaltonSequenceAllocate(
    _Out_ PLOW_DISCREPANCY_SEQUENCE *sequence
    )
{
    if (sequence == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    PSHARED_HALTON_DATA halton_data =
        (PSHARED_HALTON_DATA)malloc(sizeof(SHARED_HALTON_DATA));

    if (halton_data == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    init_faure(&halton_data->halton_sampler);
    halton_data->reference_count = 1;

    HALTON_SEQUENCE halton_sequence;
    halton_sequence.halton_data = halton_data;
    halton_sequence.num_columns = 1;
    halton_sequence.num_rows = 1;
    halton_sequence.column = 0;
    halton_sequence.row = 0;
    halton_sequence.base_index = 0;
    halton_sequence.index = 0;
    halton_sequence.dimension = 0;

    ISTATUS status = LowDiscrepancySequenceAllocate(&halton_sequence_vtable,
                                                    &halton_sequence,
                                                    sizeof(HALTON_SEQUENCE),
                                                    alignof(HALTON_SEQUENCE),
                                                    sequence);

    if (status != ISTATUS_SUCCESS)
    {
        free(halton_data);
        return status;
    }

    return ISTATUS_SUCCESS;
}