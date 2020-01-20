/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    one_light_sampler.c

Abstract:

    Implements an one light sampler

--*/

#include <stdalign.h>
#include <stdlib.h>
#include <string.h>

#include "iris_physx_toolkit/one_light_sampler.h"

//
// Types
//

typedef struct _ONE_LIGHT_SAMPLER {
    _Field_size_(num_lights) PLIGHT *lights;
    size_t num_lights;
    float_t pdf;
} ONE_LIGHT_SAMPLER, *PONE_LIGHT_SAMPLER;

typedef const ONE_LIGHT_SAMPLER *PCONE_LIGHT_SAMPLER;

//
// Static Functions
//

static
ISTATUS
OneLightSamplerSample(
    _In_opt_ const void* context,
    _In_ POINT3 hit,
    _Inout_ PRANDOM rng,
    _Inout_ PLIGHT_SAMPLE_COLLECTOR collector
    )
{
    PONE_LIGHT_SAMPLER light_sampler = (PONE_LIGHT_SAMPLER)context;

    if (light_sampler->num_lights == 0)
    {
        return ISTATUS_SUCCESS;
    }

    size_t light_index;
    ISTATUS status = RandomGenerateIndex(rng,
                                         light_sampler->num_lights,
                                         &light_index);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    status = LightSampleCollectorAddSample(collector,
                                           light_sampler->lights[light_index],
                                           light_sampler->pdf);

    return status;
}

static
ISTATUS
OneLightSamplerCacheColors(
    _In_ const void *context,
    _Inout_ PCOLOR_CACHE color_cache
    )
{
    PCONE_LIGHT_SAMPLER light_sampler = (PCONE_LIGHT_SAMPLER)context;

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
OneLightSamplerFree(
    _In_opt_ _Post_invalid_ void *context
    )
{
    PONE_LIGHT_SAMPLER light_sampler = (PONE_LIGHT_SAMPLER)context;

    for (size_t i = 0; i < light_sampler->num_lights; i++)
    {
        LightRelease(light_sampler->lights[i]);
    }

    free(light_sampler->lights);
}

//
// Static Data
//

static const LIGHT_SAMPLER_VTABLE one_light_sampler_vtable = {
    OneLightSamplerSample,
    OneLightSamplerCacheColors,
    OneLightSamplerFree
};

//
// Functions
//

ISTATUS
OneLightSamplerAllocate(
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

    ONE_LIGHT_SAMPLER result;
    if (num_lights != 0)
    {
        result.lights = (PLIGHT*)calloc(num_lights, sizeof(PLIGHT));

        if (result.lights == NULL)
        {
            return ISTATUS_ALLOCATION_FAILED;
        }

        memcpy(result.lights, lights, num_lights * sizeof(PLIGHT));
        result.pdf = (float_t)1.0 / (float_t)num_lights;
    }
    else
    {
        result.lights = NULL;
        result.pdf = (float_t)0.0;
    }

    result.num_lights = num_lights;

    ISTATUS status = LightSamplerAllocate(&one_light_sampler_vtable,
                                          &result,
                                          sizeof(ONE_LIGHT_SAMPLER),
                                          alignof(ONE_LIGHT_SAMPLER),
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