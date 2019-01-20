/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    light_sampler_internal.h

Abstract:

    The internal routines for a light sampler.

--*/

#ifndef _IRIS_PHYSX_LIGHT_SAMPLER_INTERNAL_
#define _IRIS_PHYSX_LIGHT_SAMPLER_INTERNAL_

#include "iris_physx/light_sample_collector.h"
#include "iris_physx/light_sample_collector_internal.h"
#include "iris_physx/light_sampler_vtable.h"

//
// Types
//

struct _LIGHT_SAMPLER {
    LIGHT_SAMPLE_COLLECTOR collector;
    PLIGHT_SAMPLER_SAMPLE_LIGHTS_ROUTINE sample_lights_routine;
    const void* sample_lights_context;
    size_t sample_index;
};

//
// Functions
//

static
inline
bool
LightSamplerInitialize(
    _Out_ struct _LIGHT_SAMPLER *light_sampler
    )
{
    assert(light_sampler != NULL);

    bool success = LightSampleCollectorInitialize(&light_sampler->collector);

    if (!success)
    {
        return false;
    }

    light_sampler->sample_lights_routine = NULL;
    light_sampler->sample_lights_context = NULL;
    light_sampler->sample_index = 0;

    return true;
}

static
inline
void
LightSamplerConfigure(
    _Inout_ struct _LIGHT_SAMPLER *light_sampler,
    _In_ PLIGHT_SAMPLER_SAMPLE_LIGHTS_ROUTINE sample_lights_routine,
    _In_opt_ const void* sample_lights_context
    )
{
    assert(light_sampler != NULL);
    assert(sample_lights_routine != NULL);

    LightSampleCollectorClear(&light_sampler->collector);
    light_sampler->sample_lights_routine = sample_lights_routine;
    light_sampler->sample_lights_context = sample_lights_context;
    light_sampler->sample_index = 0;
}

static
inline
void
LightSamplerDestroy(
    _Inout_ struct _LIGHT_SAMPLER *light_sampler
    )
{
    assert(light_sampler != NULL);

    LightSampleCollectorDestroy(&light_sampler->collector);
}

#endif // _IRIS_PHYSX_LIGHT_SAMPLER_INTERNAL_