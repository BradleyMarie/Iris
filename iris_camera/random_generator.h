/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    random_generator.h

Abstract:

    Generates a random number generator.

--*/

#ifndef _IRIS_CAMERA_RANDOM_GENERATOR_
#define _IRIS_CAMERA_RANDOM_GENERATOR_

#include "iris_advanced/iris_advanced.h"

//
// Types
//

ISTATUS
(*PRANDOM_GENERATOR_GENERATE_ROUTINE)(
    _In_ const void *context,
    _Out_ PRANDOM *rng
    );

typedef struct _RANDOM_GENERATOR RANDOM_GENERATOR, *PRANDOM_GENERATOR;
typedef const RANDOM_GENERATOR *PCRANDOM_GENERATOR;

//
// Functions
//

ISTATUS
RandomGeneratorAllocate(
    _In_ PRANDOM_GENERATOR_GENERATE_ROUTINE generate_routine,
    _In_reads_bytes_opt_(data_size) const void *data,
    _In_ size_t data_size,
    _In_ size_t data_alignment,
    _Out_ PRANDOM_GENERATOR *random_generator
    );

void
RandomGeneratorFree(
    _In_opt_ _Post_invalid_ PRANDOM_GENERATOR random_generator
    );

#endif // _IRIS_CAMERA_RANDOM_GENERATOR_