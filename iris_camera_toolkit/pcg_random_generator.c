/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    pcg_random.c

Abstract:

    Implements a permuted congruential random number generator.

--*/

#include <stdalign.h>

#include "iris_advanced_toolkit/pcg_random.h"
#include "iris_camera_toolkit/pcg_random_generator.h"
#include "third_party/pcg_c_basic/pcg_basic.h"

//
// Types
//

typedef struct _PCG_RANDOM_GENERATOR {
    pcg32_random_t state;
} PCG_RANDOM_GENERATOR, *PPCG_RANDOM_GENERATOR;

//
// Static Functions
//

ISTATUS
PermutedCongruentialRandomGeneratorGenerate(
    _In_ void *context,
    _Out_ PRANDOM *rng
    )
{
    PPCG_RANDOM_GENERATOR pgc_generator = (PPCG_RANDOM_GENERATOR)context;

    uint64_t state_lo = pcg32_random_r(&pgc_generator->state);
    uint64_t state_hi = pcg32_random_r(&pgc_generator->state);
    uint64_t initial_state = (state_hi << 32) | state_lo;

    uint64_t seq_lo = pcg32_random_r(&pgc_generator->state);
    uint64_t seq_hi = pcg32_random_r(&pgc_generator->state);
    uint64_t initial_output_sequence = (seq_hi << 32) | seq_lo;

    ISTATUS status = PermutedCongruentialRandomAllocate(initial_state,
                                                        initial_output_sequence,
                                                        rng);

    return status;
}

//
// Static Variables
//

static const RANDOM_GENERATOR_VTABLE pcg_generator_vtable = {
    PermutedCongruentialRandomGeneratorGenerate,
    NULL
};

//
// Functions
//

ISTATUS
PermutedCongruentiaRandomGeneratorAllocate(
    _In_ uint64_t initial_state,
    _In_ uint64_t initial_output_sequence,
    _Out_ PRANDOM_GENERATOR *generator
    )
{
    if (generator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    PCG_RANDOM_GENERATOR pcg_generator;
    pcg32_srandom_r(&pcg_generator.state,
                    initial_state,
                    initial_output_sequence);

    ISTATUS status = RandomGeneratorAllocate(&pcg_generator_vtable,
                                             &pcg_generator,
                                             sizeof(PCG_RANDOM_GENERATOR),
                                             alignof(PCG_RANDOM_GENERATOR),
                                             generator);

    return status;
}