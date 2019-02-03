/*++

Copyright (c) 2019 Brad Weinberger

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
    _In_ POINT3 point,
    _Out_ size_t *num_samples
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

    if (num_samples == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    LightSampleCollectorClear(&light_sampler->collector);

    ISTATUS status = light_sampler->sample_lights_routine(
        light_sampler->sample_lights_context,
        point,
        rng,
        &light_sampler->collector);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    *num_samples =
        LightSampleCollectorGetSampleCount(&light_sampler->collector);

    return ISTATUS_SUCCESS;
}

ISTATUS
LightSamplerGetSample(
    _In_ PCLIGHT_SAMPLER light_sampler,
    _In_ size_t sample_index,
    _Out_ PCLIGHT *light,
    _Out_ float_t *pdf
    )
{
    if (light_sampler == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    size_t num_samples =
        LightSampleCollectorGetSampleCount(&light_sampler->collector);

    if (num_samples <= sample_index)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (light == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (pdf == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    LightSampleCollectorGetSample(&light_sampler->collector,
                                  sample_index,
                                  light,
                                  pdf);

    return ISTATUS_SUCCESS;
}