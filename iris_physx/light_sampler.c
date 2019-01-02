/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    light_sampler.c

Abstract:

    Implements a light sampler.

--*/

#include "iris_physx/light_sampler.h"
#include "iris_physx/light_sampler_internal.h"

//
// Functions
//

ISTATUS
LightSamplerCollectSamples(
    _Inout_ PLIGHT_SAMPLER light_sampler,
    _Inout_ PRANDOM rng,
    _In_ POINT3 point
    )
{
    if (light_sampler == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (rng == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (!PointValidate(point))
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    LightSampleCollectorClear(&light_sampler->collector);
    light_sampler->sample_index = 0;

    ISTATUS status = light_sampler->sample_lights_routine(
        light_sampler->sample_lights_context,
        point,
        rng,
        &light_sampler->collector);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    size_t num_samples =
        LightSampleCollectorGetSampleCount(&light_sampler->collector);

    if (num_samples == 0)
    {
        return ISTATUS_DONE;
    }

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == 0 || return == 1)
ISTATUS
LightSamplerNextSample(
    _Inout_ PLIGHT_SAMPLER light_sampler,
    _Out_ PCLIGHT *light,
    _Out_ float_t *pdf
    )
{
    if (light_sampler == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (light == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (pdf == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    size_t num_samples =
        LightSampleCollectorGetSampleCount(&light_sampler->collector);

    if (num_samples <= light_sampler->sample_index)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    LightSampleCollectorGetSample(&light_sampler->collector,
                                  light_sampler->sample_index,
                                  light,
                                  pdf);

    light_sampler->sample_index += 1;

    if (num_samples == light_sampler->sample_index)
    {
        return ISTATUS_DONE;
    }

    return ISTATUS_SUCCESS;
}