/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    light_sampler_internal.h

Abstract:

    The internal routines for a light sampler.

--*/

#ifndef _IRIS_PHYSX_LIGHT_SAMPLER_INTERNAL_
#define _IRIS_PHYSX_LIGHT_SAMPLER_INTERNAL_

#include "iris_physx/light_sampler_vtable.h"

//
// Types
//

struct _LIGHT_SAMPLER {
    PLIGHT_SAMPLER_PREPARE_SAMPLES_ROUTINE prepare_samples_routine;
    PLIGHT_SAMPLER_NEXT_SAMPLE_ROUTINE next_sample_routine;
    void *data;
    bool prepared;
};

//
// Functions
//

static
inline
void
LightSamplerInitialize(
    _Out_ struct _LIGHT_SAMPLER *light_sampler,
    _In_opt_ PLIGHT_SAMPLER_PREPARE_SAMPLES_ROUTINE prepare_samples_routine,
    _In_ PLIGHT_SAMPLER_NEXT_SAMPLE_ROUTINE next_sample_routine,
    _In_opt_ void *data
    )
{
    assert(next_sample_routine != NULL);

    light_sampler->prepare_samples_routine = prepare_samples_routine;
    light_sampler->next_sample_routine = next_sample_routine;
    light_sampler->data = data;
    light_sampler->prepared = false;
}

#endif // _IRIS_PHYSX_LIGHT_SAMPLER_INTERNAL_