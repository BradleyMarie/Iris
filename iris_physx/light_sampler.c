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
LightSamplerPrepareSamples(
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

    if (light_sampler->prepare_samples_routine == NULL)
    {
        light_sampler->prepared = true;
        return ISTATUS_SUCCESS;
    }

    ISTATUS status = light_sampler->prepare_samples_routine(light_sampler->data,
                                                            rng,
                                                            point);

    if (status == ISTATUS_SUCCESS)
    {
        light_sampler->prepared = true;
    }

    return status;
}

_Check_return_
_Success_(return == 0 || return == 1)
ISTATUS
LightSamplerNextSample(
    _Inout_ PLIGHT_SAMPLER light_sampler,
    _Inout_ PRANDOM rng,
    _Out_ PCLIGHT *light,
    _Out_ float_t *pdf
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

    if (light == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (pdf == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (!light_sampler->prepared)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    ISTATUS status =  light_sampler->next_sample_routine(light_sampler->data,
                                                         rng,
                                                         light,
                                                         pdf);

    if (status == ISTATUS_DONE)
    {
        light_sampler->prepared = false;
    }

    return status;
}