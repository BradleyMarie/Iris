/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    all_light_sampler.c

Abstract:

    Implements an all light sampler

--*/

#include <stdalign.h>
#include <stdlib.h>
#include <string.h>

#include "iris_physx_toolkit/all_light_sampler.h"

//
// Types
//

typedef struct _ALL_LIGHT_SAMPLER {
    _Field_size_(num_lights) PLIGHT *lights;
    size_t num_lights;
} ALL_LIGHT_SAMPLER, *PALL_LIGHT_SAMPLER;

typedef const ALL_LIGHT_SAMPLER *PCALL_LIGHT_SAMPLER;

//
// Static Functions
//

static
ISTATUS
AllLightSamplerSample(
    _In_opt_ const void* context,
    _In_ POINT3 hit,
    _Inout_ PRANDOM rng,
    _Inout_ PLIGHT_SAMPLE_COLLECTOR collector
    )
{
    PALL_LIGHT_SAMPLER light_sampler = (PALL_LIGHT_SAMPLER)context;

    for (size_t i = 0; i < light_sampler->num_lights; i++)
    {
        ISTATUS status =
            LightSampleCollectorAddSample(collector,
                                          light_sampler->lights[i],
                                          (float_t)1.0);

        if (status != ISTATUS_SUCCESS)
        {
            return status;
        }
    }

    return ISTATUS_SUCCESS;
}

static
ISTATUS
AllLightSamplerCacheColors(
    _In_ const void *context,
    _Inout_ PCOLOR_CACHE color_cache
    )
{
    PCALL_LIGHT_SAMPLER light_sampler = (PCALL_LIGHT_SAMPLER)context;

    for (size_t i = 0; i < light_sampler->num_lights; i++)
    {
        ISTATUS status = LightCacheColors(light_sampler->lights[i],
                                          color_cache);

        if (status != ISTATUS_SUCCESS)
        {
            return status;
        }
    }

    return ISTATUS_SUCCESS;
}

static
void
AllLightSamplerFree(
    _In_opt_ _Post_invalid_ void *context
    )
{
    PALL_LIGHT_SAMPLER light_sampler = (PALL_LIGHT_SAMPLER)context;

    for (size_t i = 0; i < light_sampler->num_lights; i++)
    {
        LightRelease(light_sampler->lights[i]);
    }

    free(light_sampler->lights);
}

//
// Static Data
//

static const LIGHT_SAMPLER_VTABLE all_light_sampler_vtable = {
    AllLightSamplerSample,
    AllLightSamplerCacheColors,
    AllLightSamplerFree
};

//
// Functions
//

ISTATUS
AllLightSamplerAllocate(
    _In_reads_(num_lights) PLIGHT *lights,
    _In_ size_t num_lights,
    _Out_ PLIGHT_SAMPLER *light_sampler
    )
{
    if (light_sampler == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (lights == NULL && num_lights != 0)
    {
        return ISTATUS_INVALID_ARGUMENT_COMBINATION_00;
    }

    ALL_LIGHT_SAMPLER result;
    if (num_lights != 0)
    {
        result.lights = (PLIGHT*)calloc(num_lights, sizeof(PLIGHT));

        if (result.lights == NULL)
        {
            return ISTATUS_ALLOCATION_FAILED;
        }

        memcpy(result.lights, lights, num_lights * sizeof(PLIGHT));
    }
    else
    {
        result.lights = NULL;
    }

    result.num_lights = num_lights;

    ISTATUS status = LightSamplerAllocate(&all_light_sampler_vtable,
                                          &result,
                                          sizeof(ALL_LIGHT_SAMPLER),
                                          alignof(ALL_LIGHT_SAMPLER),
                                          light_sampler);

    if (status != ISTATUS_SUCCESS)
    {
        free(result.lights);
        return status;
    }

    for (size_t i = 0; i < num_lights; i++)
    {
        LightRetain(lights[i]);
    }

    return ISTATUS_SUCCESS;
}