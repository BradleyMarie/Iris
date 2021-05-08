/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    aggregate_environmental_light.c

Abstract:

    Implements a aggregate environmental light.

--*/

#include <stdalign.h>
#include <stdlib.h>

#include "iris_physx_toolkit/aggregate_environmental_light.h"

//
// Types
//

_Struct_size_bytes_((1 + num_bsdfs) * sizeof(PENVIRONMENTAL_LIGHT))
typedef struct _AGGREGATE_ENVIRONMENTAL_LIGHT {
    size_t num_lights;
    _Field_size_(num_lights) PENVIRONMENTAL_LIGHT lights[];
} AGGREGATE_ENVIRONMENTAL_LIGHT, *PAGGREGATE_ENVIRONMENTAL_LIGHT;

typedef const AGGREGATE_ENVIRONMENTAL_LIGHT *PCAGGREGATE_ENVIRONMENTAL_LIGHT;

//
// Static Functions
//

static
ISTATUS
AggregateEnvironmentalLightSample(
    _In_ const void *context,
    _In_ VECTOR3 surface_normal,
    _Inout_ PRANDOM rng,
    _Inout_ PSPECTRUM_COMPOSITOR compositor,
    _Out_ PCSPECTRUM *spectrum,
    _Out_ PVECTOR3 to_light,
    _Out_ float_t *pdf
    )
{
    PCAGGREGATE_ENVIRONMENTAL_LIGHT aggregate_light =
        (PCAGGREGATE_ENVIRONMENTAL_LIGHT)context;

    size_t sampled_index;
    ISTATUS status = RandomGenerateIndex(rng,
                                         aggregate_light->num_lights,
                                         &sampled_index);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    status = EnvironmentalLightSample(aggregate_light->lights[sampled_index],
                                      surface_normal,
                                      rng,
                                      compositor,
                                      spectrum,
                                      to_light,
                                      pdf);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    for (size_t i = 0; i < aggregate_light->num_lights; i++)
    {
        if (i == sampled_index)
        {
            continue;
        }

        PCSPECTRUM light_spectrum;
        float_t light_pdf;
        ISTATUS status =
            EnvironmentalLightComputeEmissiveWithPdf(aggregate_light->lights[i],
                                                     *to_light,
                                                     compositor,
                                                     &light_spectrum,
                                                     &light_pdf);

        if (status != ISTATUS_SUCCESS)
        {
            return status;
        }

        if (light_pdf <= (float_t)0.0)
        {
            continue;
        }

        status = SpectrumCompositorAddSpectra(compositor,
                                              *spectrum,
                                              light_spectrum,
                                              spectrum);

        if (status != ISTATUS_SUCCESS)
        {
            return status;
        }

        *pdf += light_pdf;
    }

    *pdf /= (float_t)aggregate_light->num_lights;

    return ISTATUS_SUCCESS;
}

static
ISTATUS
AggregateEnvironmentalLightComputeEmissive(
    _In_ const void *context,
    _In_ VECTOR3 to_light,
    _Inout_ PSPECTRUM_COMPOSITOR compositor,
    _Out_ PCSPECTRUM *spectrum
    )
{
    PCAGGREGATE_ENVIRONMENTAL_LIGHT aggregate_light =
        (PCAGGREGATE_ENVIRONMENTAL_LIGHT)context;

    *spectrum = NULL;
    for (size_t i = 0; i < aggregate_light->num_lights; i++)
    {
        PCSPECTRUM light_spectrum;
        ISTATUS status =
            EnvironmentalLightComputeEmissive(aggregate_light->lights[i],
                                              to_light,
                                              compositor,
                                              &light_spectrum);

        if (status != ISTATUS_SUCCESS)
        {
            return status;
        }

        status = SpectrumCompositorAddSpectra(compositor,
                                              *spectrum,
                                              light_spectrum,
                                              spectrum);

        if (status != ISTATUS_SUCCESS)
        {
            return status;
        }
    }

    return ISTATUS_SUCCESS;
}

static
ISTATUS
AggregateEnvironmentalLightComputeEmissiveWithPdf(
    _In_ const void *context,
    _In_ VECTOR3 to_light,
    _Inout_ PSPECTRUM_COMPOSITOR compositor,
    _Out_ PCSPECTRUM *spectrum,
    _Out_ float_t *pdf
    )
{
    PCAGGREGATE_ENVIRONMENTAL_LIGHT aggregate_light =
        (PCAGGREGATE_ENVIRONMENTAL_LIGHT)context;

    *spectrum = NULL;
    *pdf = (float_t)0.0;
    for (size_t i = 0; i < aggregate_light->num_lights; i++)
    {
        PCSPECTRUM light_spectrum;
        float_t light_pdf;
        ISTATUS status =
            EnvironmentalLightComputeEmissiveWithPdf(aggregate_light->lights[i],
                                                     to_light,
                                                     compositor,
                                                     &light_spectrum,
                                                     &light_pdf);

        if (status != ISTATUS_SUCCESS)
        {
            return status;
        }

        if (light_pdf <= (float_t)0.0)
        {
            continue;
        }

        status = SpectrumCompositorAddSpectra(compositor,
                                              *spectrum,
                                              light_spectrum,
                                              spectrum);

        if (status != ISTATUS_SUCCESS)
        {
            return status;
        }

        *pdf += light_pdf;
    }

    *pdf /= (float_t)aggregate_light->num_lights;

    return ISTATUS_SUCCESS;
}

static
void
AggregateEnvironmentalLightFree(
    _In_opt_ _Post_invalid_ void *context
    )
{
    PAGGREGATE_ENVIRONMENTAL_LIGHT light = (PAGGREGATE_ENVIRONMENTAL_LIGHT)context;

    for (size_t i = 0; i < light->num_lights; i++)
    {
        EnvironmentalLightRelease(light->lights[i]);
    }
}

//
// Static Variables
//

static const ENVIRONMENTAL_LIGHT_VTABLE aggregate_environmental_light_vtable = {
    AggregateEnvironmentalLightSample,
    AggregateEnvironmentalLightComputeEmissive,
    AggregateEnvironmentalLightComputeEmissiveWithPdf,
    AggregateEnvironmentalLightFree
};

//
// Functions
//

ISTATUS
AggregateEnvironmentalLightAllocate(
    _In_reads_(num_lights) PENVIRONMENTAL_LIGHT *lights,
    _In_ size_t num_lights,
    _Out_ PENVIRONMENTAL_LIGHT *environmental_light,
    _Out_ PLIGHT *light
    )
{
    if (lights == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (num_lights == 0)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (environmental_light == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (light == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    PAGGREGATE_ENVIRONMENTAL_LIGHT aggregate_light =
        (PAGGREGATE_ENVIRONMENTAL_LIGHT)calloc(1 + num_lights, sizeof(size_t));

    if (aggregate_light == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    size_t inserted_lights = 0;
    for (size_t i = 0; i < num_lights; i++)
    {
        if (lights[i] != NULL)
        {
            aggregate_light->lights[inserted_lights++] = lights[i];
        }
    }

    if (inserted_lights == 0)
    {
        free(aggregate_light);
        *environmental_light = NULL;
        *light = NULL;
        return ISTATUS_SUCCESS;
    }

    aggregate_light->num_lights = inserted_lights;

    ISTATUS status =
        EnvironmentalLightAllocate(&aggregate_environmental_light_vtable,
                                   aggregate_light,
                                   sizeof(PENVIRONMENTAL_LIGHT) * (1 + inserted_lights),
                                   alignof(AGGREGATE_ENVIRONMENTAL_LIGHT),
                                   environmental_light,
                                   light);

    free(aggregate_light);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    for (size_t i = 0; i < num_lights; i++)
    {
        EnvironmentalLightRetain(lights[i]);
    }

    return ISTATUS_SUCCESS;
}