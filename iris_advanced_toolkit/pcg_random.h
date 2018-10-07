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

#if __cplusplus 
extern "C" {
#endif // __cplusplus

//
// Functions
//

ISTATUS
PermutedCongruentialRandomAllocate(
    _In_ uint64_t initial_state,
    _In_ uint64_t initial_output_sequence,
    _Out_ PRANDOM *rng
    );

#if __cplusplus 
}
#endif // __cplusplus

#endif // _IRIS_ADVANCED_TOOLKIT_PCG_RANDOM_