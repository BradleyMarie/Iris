/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    environmental_light.c

Abstract:

    Definitions for the environmental light interface.

--*/

#include <stdalign.h>
#include <stdatomic.h>
#include <string.h>

#include "common/alloc.h"
#include "iris_physx/environmental_light.h"
#include "iris_physx/environmental_light_internal.h"
#include "iris_physx/visibility_tester_internal.h"

//
// Types
//

typedef struct _ENVIRONMENTAL_LIGHT_WRAPPER {
    PENVIRONMENTAL_LIGHT environment;
} ENVIRONMENTAL_LIGHT_WRAPPER, *PENVIRONMENTAL_LIGHT_WRAPPER;

typedef const ENVIRONMENTAL_LIGHT_WRAPPER *PCENVIRONMENTAL_LIGHT_WRAPPER;

//
// Static Functions
//

static
ISTATUS
EnvironmentalLightWrapperSample(
    _In_ const void *context,
    _In_ POINT3 hit_point,
    _In_ VECTOR3 surface_normal,
    _Inout_ PVISIBILITY_TESTER visibility_tester,
    _Inout_ PRANDOM rng,
    _Inout_ PSPECTRUM_COMPOSITOR compositor,
    _Out_ PCSPECTRUM *spectrum,
    _Out_ PVECTOR3 to_light,
    _Out_ float_t *pdf
    )
{
    PCENVIRONMENTAL_LIGHT_WRAPPER environmental_light_wrapper = (PCENVIRONMENTAL_LIGHT_WRAPPER)context;
    PCENVIRONMENTAL_LIGHT light = environmental_light_wrapper->environment;

    ISTATUS status = light->vtable->sample_routine(light->data,
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

    if (*spectrum == NULL || *pdf <= (float_t)0.0)
    {
        return ISTATUS_SUCCESS;
    }

    RAY ray_to_light = RayCreate(hit_point, *to_light);

    bool visible;
    status = VisibilityTesterTestAnyDistanceInline(visibility_tester,
                                                   ray_to_light,
                                                   &visible);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    if (!visible)
    {
        *pdf = (float_t)0.0;
    }

    return ISTATUS_SUCCESS;
}

static
ISTATUS
EnvironmentalLightWrapperComputeEmissive(
    _In_ const void *context,
    _In_ PCRAY to_light,
    _Inout_ PVISIBILITY_TESTER visibility_tester,
    _Inout_ PSPECTRUM_COMPOSITOR compositor,
    _Out_ PCSPECTRUM *spectrum
    )
{
    PCENVIRONMENTAL_LIGHT_WRAPPER environmental_light_wrapper = (PCENVIRONMENTAL_LIGHT_WRAPPER)context;
    PCENVIRONMENTAL_LIGHT light = environmental_light_wrapper->environment;

    ISTATUS status = EnvironmentalLightComputeEmissive(light,
                                                       to_light->direction,
                                                       compositor,
                                                       spectrum);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    if (*spectrum == NULL)
    {
        return ISTATUS_SUCCESS;
    }

    bool visible;
    status = VisibilityTesterTestAnyDistanceInline(visibility_tester,
                                                   *to_light,
                                                   &visible);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    if (!visible)
    {
        *spectrum = NULL;
    }

    return ISTATUS_SUCCESS;
}

static
ISTATUS
EnvironmentalLightWrapperComputeEmissiveWithPdf(
    _In_ const void *context,
    _In_ PCRAY to_light,
    _Inout_ PVISIBILITY_TESTER visibility_tester,
    _Inout_ PSPECTRUM_COMPOSITOR compositor,
    _Out_ PCSPECTRUM *spectrum,
    _Out_ float_t *pdf
    )
{
    PCENVIRONMENTAL_LIGHT_WRAPPER environmental_light_wrapper = (PCENVIRONMENTAL_LIGHT_WRAPPER)context;
    PCENVIRONMENTAL_LIGHT light = environmental_light_wrapper->environment;

    ISTATUS status =
        light->vtable->compute_emissive_with_pdf_routine(light->data,
                                                         to_light->direction,
                                                         compositor,
                                                         spectrum,
                                                         pdf);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    if (*spectrum == NULL || *pdf == (float_t)0.0)
    {
        return ISTATUS_SUCCESS;
    }

    bool visible;
    status = VisibilityTesterTestAnyDistanceInline(visibility_tester,
                                                   *to_light,
                                                   &visible);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    if (!visible)
    {
        *pdf = (float_t)0.0;
    }

    return ISTATUS_SUCCESS;
}

static
void
EnvironmentalLightWrapperFree(
    _In_opt_ _Post_invalid_ void *context
    )
{
    PENVIRONMENTAL_LIGHT_WRAPPER environmental_light_wrapper = (PENVIRONMENTAL_LIGHT_WRAPPER)context;

    EnvironmentalLightRelease(environmental_light_wrapper->environment);
}

//
// Static Variables
//

static const LIGHT_VTABLE environmental_light_wrapper_vtable = {
    EnvironmentalLightWrapperSample,
    EnvironmentalLightWrapperComputeEmissive,
    EnvironmentalLightWrapperComputeEmissiveWithPdf,
    EnvironmentalLightWrapperFree
};

static
ISTATUS
EnvironmentalLightWrapperAllocate(
    _In_opt_ PENVIRONMENTAL_LIGHT environment,
    _Out_ PLIGHT *light
    )
{
    assert(light != NULL);

    if (environment == NULL)
    {
        *light = NULL;
        return ISTATUS_SUCCESS;
    }

    ENVIRONMENTAL_LIGHT_WRAPPER environmental_light_wrapper;
    environmental_light_wrapper.environment = environment;

    ISTATUS status = LightAllocate(&environmental_light_wrapper_vtable,
                                   &environmental_light_wrapper,
                                   sizeof(ENVIRONMENTAL_LIGHT_WRAPPER),
                                   alignof(ENVIRONMENTAL_LIGHT_WRAPPER),
                                   light);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    EnvironmentalLightRetain(environment);

    return ISTATUS_SUCCESS;
}

//
// Functions
//

ISTATUS
EnvironmentalLightAllocate(
    _In_ PCENVIRONMENTAL_LIGHT_VTABLE vtable,
    _In_reads_bytes_opt_(data_size) const void *data,
    _In_ size_t data_size,
    _In_ size_t data_alignment,
    _Out_ PENVIRONMENTAL_LIGHT *environmental_light,
    _Out_ PLIGHT *light
    )
{
    if (vtable == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (data_size != 0)
    {
        if (data == NULL)
        {
            return ISTATUS_INVALID_ARGUMENT_COMBINATION_00;
        }
        
        if (data_alignment == 0 ||
            (data_alignment & (data_alignment - 1)) != 0)
        {
            return ISTATUS_INVALID_ARGUMENT_COMBINATION_01;    
        }
        
        if (data_size % data_alignment != 0)
        {
            return ISTATUS_INVALID_ARGUMENT_COMBINATION_02;
        }
    }

    if (environmental_light == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    if (light == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }

    void *data_allocation;
    bool success = AlignedAllocWithHeader(sizeof(ENVIRONMENTAL_LIGHT),
                                          alignof(ENVIRONMENTAL_LIGHT),
                                          (void **)environmental_light,
                                          data_size,
                                          data_alignment,
                                          &data_allocation);

    if (!success)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    (*environmental_light)->vtable = vtable;
    (*environmental_light)->data = data_allocation;
    (*environmental_light)->reference_count = 1;

    if (data_size != 0)
    {
        memcpy(data_allocation, data, data_size);
    }

    ISTATUS status = EnvironmentalLightWrapperAllocate(*environmental_light,
                                                       light);

    if (status != ISTATUS_SUCCESS)
    {
        free(*environmental_light);
        *environmental_light = NULL;
    }

    return status;
}

void
EnvironmentalLightRetain(
    _In_opt_ PENVIRONMENTAL_LIGHT environmental_light
    )
{
    if (environmental_light == NULL)
    {
        return;
    }

    atomic_fetch_add(&environmental_light->reference_count, 1);
}

void
EnvironmentalLightRelease(
    _In_opt_ _Post_invalid_ PENVIRONMENTAL_LIGHT environmental_light
    )
{
    if (environmental_light == NULL)
    {
        return;
    }

    if (atomic_fetch_sub(&environmental_light->reference_count, 1) == 1)
    {
        if (environmental_light->vtable->free_routine != NULL)
        {
            environmental_light->vtable->free_routine(environmental_light->data);
        }
    
        free(environmental_light);
    }
}