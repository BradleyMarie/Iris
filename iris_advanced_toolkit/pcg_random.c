/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    pcg_random.c

Abstract:

    Implements a permuted congruential random number generator.

--*/

#include <assert.h>
#include <float.h>
#include <stdalign.h>

#include "iris_advanced_toolkit/pcg_random.h"
#include "third_party/pcg_c_basic/pcg_basic.h"

//
// Types
//

typedef struct _PCG_RANDOM {
    pcg32_random_t state;
} PCG_RANDOM, *PPCG_RANDOM;

//
// Static Functions
//

static
ISTATUS
PermutedCongruentialRandomGenerateFloat(
    _In_ void *context,
    _In_ float_t minimum,
    _In_ float_t maximum,
    _Out_range_(minimum, maximum) float_t *result
    )
{
    static_assert(FLT_EVAL_METHOD == 0,
                  "pcg_random only supports float_t as float");
    static_assert(FLT_RADIX == 2 && FLT_MANT_DIG - 1 < sizeof(uint32_t) * 8,
                  "float mantissa must be less than sizeof(uint32_t)");

    PPCG_RANDOM pcg_random = (PPCG_RANDOM)context;
    uint32_t random_uint = pcg32_boundedrand_r(&pcg_random->state,
                                               1u << (FLT_MANT_DIG - 1u));
    float_t random_float = ldexp((float_t)random_uint, -FLT_MANT_DIG + 1);

    *result = fma(maximum - minimum, random_float, minimum);

    return ISTATUS_SUCCESS;
}

static
ISTATUS
PermutedCongruentialRandomGenerateIndex(
    _In_ void *context,
    _In_ size_t upper_bound,
    _Out_range_(0, upper_bound - 1) size_t *result
    )
{
    if (UINT32_MAX < upper_bound)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    PPCG_RANDOM pcg_random = (PPCG_RANDOM)context;

    *result = pcg32_boundedrand_r(&pcg_random->state, upper_bound);

    return ISTATUS_SUCCESS;
}

ISTATUS
PermutedCongruentialRandomReplicate(
    _In_opt_ void *context,
    _Out_ PRANDOM *replica
    )
{
    PPCG_RANDOM pcg_random = (PPCG_RANDOM)context;

    uint32_t initial_state_l = pcg32_random_r(&pcg_random->state);
    uint32_t initial_state_h = pcg32_random_r(&pcg_random->state);
    uint32_t initial_output_sequence_l = pcg32_random_r(&pcg_random->state);
    uint32_t initial_output_sequence_h = pcg32_random_r(&pcg_random->state);

    uint64_t initial_state =
        initial_state_l | ((uint64_t)initial_state_h << 32);
    uint64_t initial_output_sequence =
        initial_output_sequence_l | ((uint64_t)initial_output_sequence_h << 32);

    ISTATUS status = PermutedCongruentialRandomAllocate(initial_state,
                                                        initial_output_sequence,
                                                        replica);

    return status;
}

//
// Static Variables
//

static const RANDOM_VTABLE pcg_vtable = {
    PermutedCongruentialRandomGenerateFloat,
    PermutedCongruentialRandomGenerateIndex,
    PermutedCongruentialRandomReplicate,
    NULL
};

//
// Functions
//

ISTATUS
PermutedCongruentialRandomAllocate(
    _In_ uint64_t initial_state,
    _In_ uint64_t initial_output_sequence,
    _Out_ PRANDOM *rng
    )
{
    if (rng == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    PCG_RANDOM pcg;
    pcg32_srandom_r(&pcg.state, initial_state, initial_output_sequence);

    ISTATUS status = RandomAllocate(&pcg_vtable,
                                    &pcg,
                                    sizeof(PCG_RANDOM),
                                    alignof(PCG_RANDOM),
                                    rng);

    return status;
}