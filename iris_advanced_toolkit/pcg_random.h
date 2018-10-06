/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    pcg_random.h

Abstract:

    Creates a permuted congruential random number generator.

--*/

#ifndef _IRIS_ADVANCED_TOOLKIT_PCG_RANDOM_
#define _IRIS_ADVANCED_TOOLKIT_PCG_RANDOM_

#include "iris_advanced/iris_advanced.h"

//
// Functions
//

ISTATUS
PermutedCongruentialGeneratorAllocate(
    _In_ uint64_t initial_state,
    _In_ uint64_t initial_output_sequence,
    _Out_ PRANDOM *rng
    );

#endif // _IRIS_ADVANCED_TOOLKIT_PCG_RANDOM_