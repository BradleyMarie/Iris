/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    sobol_sequence.c

Abstract:

    Implements a Sobol image sampler.

--*/

#include <stdalign.h>

#include "iris_advanced_toolkit/sobol_sequence.h"
#include "third_party/gruenschloss/double/sobol.h"
#include "third_party/gruenschloss/single/sobol.h"
#include "third_party/pbrt-v3/sobolmatrices.h"

//
// Types
//

typedef struct _SOBOL_SEQUENCE {
    double to_first_dimension;
    double to_second_dimension;
    uint64_t logical_resolution_log2;
    unsigned num_columns;
    unsigned num_rows;
    unsigned index;
    unsigned dimension;
} SOBOL_SEQUENCE, *PSOBOL_SEQUENCE;

//
// Static Inline Functions
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
    _In_ uint64_t resolution_log2,
    _In_ uint64_t column,
    _In_ uint64_t row,
    _In_ uint64_t sample
    )
{
    if (resolution_log2 == 0)
    {
        return sample;
    }

    size_t matrix_index = (size_t)resolution_log2 - 1;

    uint64_t transformed_sample_index =
        BitMatrixVectorMultiply(VdCSobolMatrices[matrix_index],
                                sample);

    uint64_t pixel_index = (column << resolution_log2) | row;
    uint64_t image_sample_index = pixel_index ^ transformed_sample_index;        

    uint64_t transformed_image_sample_index =
        BitMatrixVectorMultiply(VdCSobolMatricesInv[matrix_index],
                                image_sample_index);

    uint64_t num_pixels_log_2 = resolution_log2 << 1;
    uint64_t sample_index_base = sample << num_pixels_log_2;

    return sample_index_base ^ transformed_image_sample_index;
}

//
// Static Functions
//

static
ISTATUS
SobolSequenceComputeIndex(
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

    PSOBOL_SEQUENCE sobol_sequence = (PSOBOL_SEQUENCE)context;

    if (sobol_sequence->num_columns != num_columns ||
        sobol_sequence->num_rows != num_rows)
    {
        size_t max_dimension =
            (num_columns < num_rows) ? num_rows : num_columns;
        uint64_t resolution = RoundUpToPowerOfTwo(max_dimension);

        sobol_sequence->logical_resolution_log2 = UInt64Log2(resolution);
        sobol_sequence->num_columns = num_columns;
        sobol_sequence->num_rows = num_rows;

        sobol_sequence->to_first_dimension =
            (double)resolution / (double)num_columns;
        sobol_sequence->to_second_dimension =
            (double)resolution / (double)num_rows;
    }

    *index =
        SobolSequenceIndex(sobol_sequence->logical_resolution_log2,
                           column,
                           row,
                           sample);

    return ISTATUS_SUCCESS;
}

static
ISTATUS
SobolSequenceStart(
    _In_ void *context,
    _In_ uint64_t index
    )
{
    if (UINT32_MAX < index)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    PSOBOL_SEQUENCE sobol_sequence = (PSOBOL_SEQUENCE)context;

    sobol_sequence->index = index;
    sobol_sequence->dimension = 0;

    return ISTATUS_SUCCESS;
}

static
ISTATUS
SobolSequenceNextFloat(
    _In_ void *context,
    _Out_ float_t *value
    )
{
    PSOBOL_SEQUENCE sobol_sequence = (PSOBOL_SEQUENCE)context;
 
    *value = sobol_single_sample(sobol_sequence->index,
                                 sobol_sequence->dimension,
                                 SOBOL_SINGLE_DEFAULT_SCRAMBLE);

    sobol_sequence->dimension += 1;

    if (sobol_sequence->dimension > 2)
    {
        return ISTATUS_SUCCESS;
    }

    if (sobol_sequence->dimension == 1)
    {
        *value *= sobol_sequence->to_first_dimension;
    }
    else if (sobol_sequence->dimension == 2)
    {
        *value *= sobol_sequence->to_second_dimension;
    }

    return ISTATUS_SUCCESS;
}

static
ISTATUS
SobolSequenceNextDouble(
    _In_ void *context,
    _Out_ double_t *value
    )
{
    PSOBOL_SEQUENCE sobol_sequence = (PSOBOL_SEQUENCE)context;

    *value = sobol_double_sample(sobol_sequence->index,
                                 sobol_sequence->dimension,
                                 SOBOL_DOUBLE_DEFAULT_SCRAMBLE);

    sobol_sequence->dimension += 1;

    if (sobol_sequence->dimension > 2)
    {
        return ISTATUS_SUCCESS;
    }

    if (sobol_sequence->dimension == 1)
    {
        *value *= sobol_sequence->to_first_dimension;
    }
    else if (sobol_sequence->dimension == 2)
    {
        *value *= sobol_sequence->to_second_dimension;
    }

    return ISTATUS_SUCCESS;
}

static
ISTATUS
SobolSequenceDuplicate(
    _In_ const void *context,
    _Out_ PLOW_DISCREPANCY_SEQUENCE *duplicate
    );

//
// Static Data
//

static const LOW_DISCREPANCY_SEQUENCE_VTABLE sobol_sequence_vtable = {
    NULL,
    SobolSequenceComputeIndex,
    SobolSequenceStart,
    SobolSequenceNextFloat,
    SobolSequenceNextDouble,
    SobolSequenceDuplicate,
    NULL
};

//
// Static Functions
//

static
ISTATUS
SobolSequenceDuplicate(
    _In_ const void *context,
    _Out_ PLOW_DISCREPANCY_SEQUENCE *duplicate
    )
{
    ISTATUS status = LowDiscrepancySequenceAllocate(&sobol_sequence_vtable,
                                                    context,
                                                    sizeof(SOBOL_SEQUENCE),
                                                    alignof(SOBOL_SEQUENCE),
                                                    duplicate);

    return status;
}

//
// Functions
//

ISTATUS
SobolSequenceAllocate(
    _Out_ PLOW_DISCREPANCY_SEQUENCE *sequence
    )
{
    if (sequence == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    SOBOL_SEQUENCE sobol_sequence;
    sobol_sequence.to_first_dimension = 1.0;
    sobol_sequence.to_second_dimension = 1.0;
    sobol_sequence.logical_resolution_log2 = 0;
    sobol_sequence.num_columns = 1;
    sobol_sequence.num_rows = 1;
    sobol_sequence.index = 0;
    sobol_sequence.dimension = 0;

    ISTATUS status = LowDiscrepancySequenceAllocate(&sobol_sequence_vtable,
                                                    &sobol_sequence,
                                                    sizeof(SOBOL_SEQUENCE),
                                                    alignof(SOBOL_SEQUENCE),
                                                    sequence);

    return status;
}