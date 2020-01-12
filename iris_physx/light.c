/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    light.c

Abstract:

    Definitions for the light interface.

--*/

#include <stdalign.h>
#include <stdatomic.h>
#include <string.h>

#include "common/alloc.h"
#include "iris_physx/light.h"
#include "iris_physx/light_internal.h"

//
// Functions
//

ISTATUS
LightAllocate(
    _In_ PCLIGHT_VTABLE vtable,
    _In_reads_bytes_opt_(data_size) const void *data,
    _In_ size_t data_size,
    _In_ size_t data_alignment,
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

    if (light == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    void *data_allocation;
    bool success = AlignedAllocWithHeader(sizeof(LIGHT),
                                          alignof(LIGHT),
                                          (void **)light,
                                          data_size,
                                          data_alignment,
                                          &data_allocation);

    if (!success)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    (*light)->vtable = vtable;
    (*light)->data = data_allocation;
    (*light)->reference_count = 1;

    if (data_size != 0)
    {
        memcpy(data_allocation, data, data_size);
    }

    return ISTATUS_SUCCESS;
}

ISTATUS
LightSample(
    _In_ PCLIGHT light,
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
    if (light == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (!PointValidate(hit_point))
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (!VectorValidate(surface_normal))
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (visibility_tester == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (rng == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    if (compositor == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }

    if (spectrum == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_06;
    }

    if (to_light == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_07;
    }

    if (pdf == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_08;
    }

    ISTATUS status = light->vtable->sample_routine(light->data,
                                                   hit_point,
                                                   surface_normal,
                                                   visibility_tester,
                                                   rng,
                                                   compositor,
                                                   spectrum,
                                                   to_light,
                                                   pdf);

    return status;
}

ISTATUS
LightComputeEmissive(
    _In_ PCLIGHT light,
    _In_ RAY to_light,
    _Inout_ PVISIBILITY_TESTER visibility_tester,
    _Inout_ PSPECTRUM_COMPOSITOR compositor,
    _Out_ PCSPECTRUM *spectrum
    )
{
    if (light == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (!RayValidate(to_light))
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (visibility_tester == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (compositor == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (spectrum == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    ISTATUS status = light->vtable->compute_emissive_routine(light->data,
                                                             &to_light,
                                                             visibility_tester,
                                                             compositor,
                                                             spectrum);

    return status;
}

ISTATUS
LightComputeEmissiveWithPdf(
    _In_ PCLIGHT light,
    _In_ RAY to_light,
    _Inout_ PVISIBILITY_TESTER visibility_tester,
    _Inout_ PSPECTRUM_COMPOSITOR compositor,
    _Out_ PCSPECTRUM *spectrum,
    _Out_ float_t *pdf
    )
{
    if (light == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (!RayValidate(to_light))
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (visibility_tester == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (compositor == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (spectrum == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    if (pdf == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }

    ISTATUS status =
        light->vtable->compute_emissive_with_pdf_routine(light->data,
                                                         &to_light,
                                                         visibility_tester,
                                                         compositor,
                                                         spectrum,
                                                         pdf);

    return status;
}

ISTATUS
LightCacheColors(
    _In_ PCLIGHT light,
    _In_ PCOLOR_CACHE color_cache
    )
{
    if (light == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (color_cache == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    ISTATUS status = light->vtable->cache_colors_routine(light->data,
                                                         color_cache);

    return status;
}

void
LightRetain(
    _In_opt_ PLIGHT light
    )
{
    if (light == NULL)
    {
        return;
    }

    atomic_fetch_add(&light->reference_count, 1);
}

void
LightRelease(
    _In_opt_ _Post_invalid_ PLIGHT light
    )
{
    if (light == NULL)
    {
        return;
    }

    if (atomic_fetch_sub(&light->reference_count, 1) == 1)
    {
        if (light->vtable->free_routine != NULL)
        {
            light->vtable->free_routine(light->data);
        }
    
        free(light);
    }
}