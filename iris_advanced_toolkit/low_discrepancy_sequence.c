/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    halton_image_sampler.c

Abstract:

    Implements a halton image sampler.

--*/

#include <stdalign.h>
#include <stdlib.h>
#include <string.h>

#include "common/alloc.h"
#include "common/safe_math.h"
#include "iris_advanced_toolkit/low_discrepancy_sequence.h"

//
// Types
//

struct _LOW_DISCREPANCY_SEQUENCE {
    PCLOW_DISCREPANCY_SEQUENCE_VTABLE vtable;
    void *data;
};

//
// Functions
//

ISTATUS
LowDiscrepancySequenceAllocate(
    _In_ PCLOW_DISCREPANCY_SEQUENCE_VTABLE vtable,
    _In_reads_bytes_opt_(data_size) const void *data,
    _In_ size_t data_size,
    _In_ size_t data_alignment,
    _Out_ PLOW_DISCREPANCY_SEQUENCE *sequence
    )
{
    if (vtable == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (data_size != 0)
    {
        if (data == NULL)
        {
            return ISTATUS_INVALID_ARGUMENT_COMBINATION_00;
        }
        
        if (data_alignment == 0 ||
            (data_alignment & (data_alignment - 1)) != 0)
        {
            return ISTATUS_INVALID_ARGUMENT_COMBINATION_01;    
        }
        
        if (data_size % data_alignment != 0)
        {
            return ISTATUS_INVALID_ARGUMENT_COMBINATION_02;
        }
    }

    if (sequence == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    void *data_allocation;
    bool success = AlignedAllocWithHeader(sizeof(LOW_DISCREPANCY_SEQUENCE),
                                          alignof(LOW_DISCREPANCY_SEQUENCE),
                                          (void **)sequence,
                                          data_size,
                                          data_alignment,
                                          &data_allocation);

    if (!success)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    (*sequence)->vtable = vtable;
    (*sequence)->data = data_allocation;

    if (data_size != 0)
    {
        memcpy(data_allocation, data, data_size);
    }

    return ISTATUS_SUCCESS;
}

ISTATUS
LowDiscrepancySequencePermute(
    _Inout_ PLOW_DISCREPANCY_SEQUENCE sequence,
    _In_ PRANDOM rng
    )
{
    if (sequence == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (rng == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (sequence->vtable->permute_routine == NULL)
    {
        return ISTATUS_SUCCESS;
    }

    ISTATUS status = sequence->vtable->permute_routine(sequence->data,
                                                       rng);

    return status;
}

ISTATUS
LowDiscrepancySequenceComputeIndex(
    _In_ PLOW_DISCREPANCY_SEQUENCE sequence,
    _In_ size_t column,
    _In_ size_t num_columns,
    _In_ size_t row,
    _In_ size_t num_rows,
    _In_ uint32_t sample,
    _In_ uint32_t num_samples,
    _Out_ uint64_t *index
    )
{
    if (sequence == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (num_columns == 0)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (column >= num_columns)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (num_rows == 0)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    if (row >= num_rows)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (num_samples == 0)
    {
        return ISTATUS_INVALID_ARGUMENT_06;
    }

    if (sample >= num_samples)
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }

    if (index == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_07;
    }

    ISTATUS status = sequence->vtable->compute_index_routine(sequence->data,
                                                             column,
                                                             num_columns,
                                                             row,
                                                             num_rows,
                                                             sample,
                                                             num_samples,
                                                             index);

    return status;
}

ISTATUS
LowDiscrepancySequenceStart(
    _In_ PLOW_DISCREPANCY_SEQUENCE sequence,
    _In_ uint64_t index
    )
{
    if (sequence == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    ISTATUS status = sequence->vtable->start_routine(sequence->data, index);

    return status;
}

ISTATUS
LowDiscrepancySequenceNextFloat(
    _In_ PLOW_DISCREPANCY_SEQUENCE sequence,
    _Out_ float_t *value
    )
{
    if (sequence == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (value == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    ISTATUS status = sequence->vtable->next_float_routine(sequence->data,
                                                          value);

    return status;
}

ISTATUS
LowDiscrepancySequenceNextDouble(
    _In_ PLOW_DISCREPANCY_SEQUENCE sequence,
    _Out_ double_t *value
    )
{
    if (sequence == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (value == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    ISTATUS status = sequence->vtable->next_double_routine(sequence->data,
                                                           value);

    return status;
}

ISTATUS
LowDiscrepancySequenceDuplicate(
    _In_ PCLOW_DISCREPANCY_SEQUENCE sequence,
    _Out_ PLOW_DISCREPANCY_SEQUENCE *duplicate
    )
{
    if (sequence == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (duplicate == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    ISTATUS status = sequence->vtable->duplicate_routine(sequence->data,
                                                         duplicate);

    return status;
}

void
LowDiscrepancySequenceFree(
    _In_opt_ _Post_invalid_ PLOW_DISCREPANCY_SEQUENCE sequence
    )
{
    if (sequence == NULL)
    {
        return;
    }

    if (sequence->vtable->free_routine != NULL)
    {
        sequence->vtable->free_routine(sequence->data);
    }

    free(sequence);
}