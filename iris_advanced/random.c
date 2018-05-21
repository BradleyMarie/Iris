/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    random.c

Abstract:

    Interface for random number generation.

--*/

#include "iris_advanced/random.h"

//
// Types
//

struct _RANDOM {
    PGENERATE_FLOAT_ROUTINE generate_float;
    PGENERATE_INDEX_ROUTINE generate_index;
    void *data;
};

//
// Functions
//

ISTATUS
RandomCreate(
    _In_ PGENERATE_FLOAT_ROUTINE generate_float_routine,
    _In_ PGENERATE_INDEX_ROUTINE generate_index_routine,
    _Inout_opt_ void *rng_context,
    _In_ PRANDOM_LIFETIME_ROUTINE callback,
    _Inout_opt_ void *callback_context
    )
{
    if (generate_float_routine == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }
    if (generate_index_routine == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (callback == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    RANDOM rng;
    rng.generate_float = generate_float_routine;
    rng.generate_index = generate_index_routine;
    rng.data = rng_context;

    return callback(callback_context, &rng);
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

    ISTATUS status = rng->generate_float(rng->data,
                                         minimum,
                                         maximum,
                                         result);

    return status;
}

ISTATUS
RandomGenerateIndex(
    _In_ PRANDOM rng,
    _In_ size_t minimum,
    _In_ size_t maximum,
    _Out_range_(minimum, maximum) size_t *result
    )
{
    if (rng == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (minimum > maximum)
    {
        return ISTATUS_INVALID_ARGUMENT_COMBINATION_00;
    }

    if (result == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    ISTATUS status = rng->generate_index(rng->data,
                                         minimum,
                                         maximum,
                                         result);

    return status;
}