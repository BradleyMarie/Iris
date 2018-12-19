/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    random.c

Abstract:

    Interface for random number generation.

--*/

#include <stdalign.h>
#include <string.h>

#include "common/alloc.h"
#include "iris_advanced/random.h"

//
// Types
//

struct _RANDOM {
    PCRANDOM_VTABLE vtable;
    void *data;
};

//
// Functions
//

ISTATUS
RandomAllocate(
    _In_ PCRANDOM_VTABLE vtable,
    _In_reads_bytes_opt_(data_size) const void *data,
    _In_ size_t data_size,
    _In_ size_t data_alignment,
    _Out_ PRANDOM *rng
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

    if (rng == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    void *data_allocation;
    bool success = AlignedAllocWithHeader(sizeof(RANDOM),
                                          alignof(RANDOM),
                                          (void **)rng,
                                          data_size,
                                          data_alignment,
                                          &data_allocation);

    if (!success)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    (*rng)->vtable = vtable;
    (*rng)->data = data_allocation;

    if (data_size != 0)
    {
        memcpy(data_allocation, data, data_size);
    }

    return ISTATUS_SUCCESS;
}

ISTATUS
RandomGenerateFloat(
    _In_ PRANDOM rng,
    _In_ float_t minimum,
    _In_ float_t maximum,
    _Out_range_(minimum, maximum) float_t *result
    )
{
    if (rng == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (!isfinite(minimum))
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (!isfinite(maximum))
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (minimum > maximum)
    {
        return ISTATUS_INVALID_ARGUMENT_COMBINATION_00;
    }

    if (result == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    ISTATUS status = rng->vtable->generate_float_routine(rng->data,
                                                         minimum,
                                                         maximum,
                                                         result);

    // Should these be made into something stronger than assertions?
    assert(minimum <= *result);
    assert(*result <= maximum);

    return status;
}

ISTATUS
RandomGenerateIndex(
    _In_ PRANDOM rng,
    _In_ size_t upper_bound,
    _Out_range_(0, upper_bound - 1) size_t *result
    )
{
    if (rng == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (upper_bound == 0)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (result == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    ISTATUS status = rng->vtable->generate_index_routine(rng->data,
                                                         upper_bound,
                                                         result);

    // Should this be made into something stronger than assertions?
    assert(*result < upper_bound);

    return status;
}

void
RandomFree(
    _In_opt_ _Post_invalid_ PRANDOM rng
    )
{
    if (rng == NULL)
    {
        return;
    }

    if (rng->vtable->free_routine)
    {
        rng->vtable->free_routine(rng->data);
    }

    free(rng);
}