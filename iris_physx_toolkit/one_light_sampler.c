/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    one_light_sampler.c

Abstract:

    Implements an all light sampler

--*/

#include "common/pointer_list.h"
#include "iris_physx_toolkit/one_light_sampler.h"

//
// Types
//

struct _ONE_LIGHT_SAMPLER {
    POINTER_LIST lights;
};

//
// Functions
//

ISTATUS
OneLightSamplerAllocate(
    _Out_ PONE_LIGHT_SAMPLER *light_sampler
    )
{
    if (light_sampler == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    PONE_LIGHT_SAMPLER result =
        (PONE_LIGHT_SAMPLER)malloc(sizeof(ONE_LIGHT_SAMPLER));

    if (result == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    if (!PointerListInitialize(&result->lights))
    {
        free(result);
        return ISTATUS_ALLOCATION_FAILED;
    }

    *light_sampler = result;

    return ISTATUS_SUCCESS;
}

ISTATUS
OneLightSamplerAddLight(
    _Inout_ PONE_LIGHT_SAMPLER light_sampler,
    _In_ PLIGHT light
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

    if (!PointerListAddPointer(&light_sampler->lights, light))
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    LightRetain(light);

    return ISTATUS_SUCCESS;
}

void
OneLightSamplerFree(
    _In_opt_ _Post_invalid_ PONE_LIGHT_SAMPLER light_sampler
    )
{
    if (light_sampler == NULL)
    {
        return;
    }

    size_t num_lights = PointerListGetSize(&light_sampler->lights);
    for (size_t i = 0; i < num_lights; i++)
    {
        PLIGHT light =
            (PLIGHT)PointerListRetrieveAtIndex(&light_sampler->lights, i);

        LightRelease(light);
    }

    PointerListDestroy(&light_sampler->lights);
    free(light_sampler);
}

//
// Callbacks
//

ISTATUS
OneLightSamplerSampleLightsCallback(
    _In_opt_ const void* context,
    _In_ POINT3 hit,
    _Inout_ PRANDOM rng,
    _Inout_ PLIGHT_SAMPLE_COLLECTOR collector
    )
{
    PONE_LIGHT_SAMPLER one_light_sampler = (PONE_LIGHT_SAMPLER)context;

    size_t num_lights = PointerListGetSize(&one_light_sampler->lights);

    size_t light_index;
    ISTATUS status = RandomGenerateIndex(rng, num_lights, &light_index);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    PCLIGHT light =
        (PCLIGHT)PointerListRetrieveAtIndex(&one_light_sampler->lights,
                                            light_index);

    float_t pdf = (float_t)1.0 / (float_t)num_lights;

    status = LightSampleCollectorAddSample(collector, light, pdf);

    return status;
}