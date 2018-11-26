/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    all_light_sampler.c

Abstract:

    Implements an all light sampler

--*/

#include "common/pointer_list.h"
#include "iris_physx_toolkit/all_light_sampler.h"

//
// Types
//

struct _ALL_LIGHT_SAMPLER {
    POINTER_LIST lights;
    size_t next_light;
};

//
// Functions
//

ISTATUS
AllLightSamplerAllocate(
    _Out_ PALL_LIGHT_SAMPLER *light_sampler
    )
{
    if (light_sampler == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    PALL_LIGHT_SAMPLER result =
        (PALL_LIGHT_SAMPLER)malloc(sizeof(ALL_LIGHT_SAMPLER));

    if (result == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    if (!PointerListInitialize(&result->lights))
    {
        free(result);
        return ISTATUS_ALLOCATION_FAILED;
    }

    result->next_light = 0;

    *light_sampler = result;

    return ISTATUS_SUCCESS;
}

ISTATUS
AllLightSamplerAddLight(
    _Inout_ PALL_LIGHT_SAMPLER light_sampler,
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
AllLightSamplerFree(
    _In_opt_ _Post_invalid_ PALL_LIGHT_SAMPLER light_sampler
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
AllLightSamplerPrepareSamplesCallback(
    _Inout_opt_ void *context,
    _Inout_ PRANDOM rng,
    _In_ POINT3 point
    )
{
    PALL_LIGHT_SAMPLER all_light_sampler = (PALL_LIGHT_SAMPLER)context;

    if (PointerListGetSize(&all_light_sampler->lights) == 0)
    {
        return ISTATUS_DONE;
    }

    all_light_sampler->next_light = 0;

    return ISTATUS_SUCCESS;
}

ISTATUS
AllLightSamplerNextSampleCallback(
    _Inout_opt_ void *context,
    _Inout_ PRANDOM rng,
    _Out_ PCLIGHT *light,
    _Out_ float_t *pdf
    )
{
    PALL_LIGHT_SAMPLER all_light_sampler = (PALL_LIGHT_SAMPLER)context;

    *light =
        (PCLIGHT)PointerListRetrieveAtIndex(&all_light_sampler->lights,
                                            all_light_sampler->next_light);
    *pdf = (float_t)1.0;

    all_light_sampler->next_light += 1;

    size_t num_lights = PointerListGetSize(&all_light_sampler->lights);

    if (all_light_sampler->next_light == num_lights)
    {
        return ISTATUS_DONE;
    }

    return ISTATUS_SUCCESS;
}