/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    pcg_random_generator.h

Abstract:

    Creates a generator for permuted congruential random number generators. 
    
    The initial_state and initial_output_sequence passed here will not be used 
    to directly seed the generated random number generators. Instead, they will
    be used to seed a separate generator that will be used to create the seeds
    for each of the allocated random number generators.

--*/

#ifndef _IRIS_CAMERA_TOOLKIT_PCG_RANDOM_GENERATOR_
#define _IRIS_CAMERA_TOOLKIT_PCG_RANDOM_GENERATOR_

#include "iris_camera/iris_camera.h"

//
// Functions
//

ISTATUS
PermutedCongruentiaRandomGeneratorAllocate(
    _In_ uint64_t initial_state,
    _In_ uint64_t initial_output_sequence,
    _Out_ PRANDOM_GENERATOR *generator
    );

#endif // _IRIS_CAMERA_TOOLKIT_PCG_RANDOM_GENERATOR_