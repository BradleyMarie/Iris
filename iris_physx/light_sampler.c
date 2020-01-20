/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    light_sampler.c

Abstract:

    Implements a light sampler.

--*/

#include <stdalign.h>
#include <stdatomic.h>
#include <string.h>

#include "common/alloc.h"
#include "iris_physx/light_sample_list_internal.h"
#include "iris_physx/light_sampler.h"

//
// Types
//

struct _LIGHT_SAMPLER {
    PCLIGHT_SAMPLER_VTABLE vtable;
    void *data;
    atomic_uintmax_t reference_count;
};

//
// Functions
//

ISTATUS
LightSamplerAllocate(
    _In_ PCLIGHT_SAMPLER_VTABLE vtable,
    _In_reads_bytes_opt_(data_size) const void *data,
    _In_ size_t data_size,
    _In_ size_t data_alignment,
    _Out_ PLIGHT_SAMPLER *light_sampler
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

    if (light_sampler == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    void *data_allocation;
    bool success = AlignedAllocWithHeader(sizeof(LIGHT_SAMPLER),
                                          alignof(LIGHT_SAMPLER),
                                          (void **)light_sampler,
                                          data_size,
                                          data_alignment,
                                          &data_allocation);

    if (!success)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    (*light_sampler)->vtable = vtable;
    (*light_sampler)->data = data_allocation;
    (*light_sampler)->reference_count = 1;

    if (data_size != 0)
    {
        memcpy(data_allocation, data, data_size);
    }

    return ISTATUS_SUCCESS;
}

ISTATUS
LightSamplerSample(
    _In_ PCLIGHT_SAMPLER light_sampler,
    _In_ POINT3 point,
    _Inout_ PRANDOM rng,
    _Inout_ PLIGHT_SAMPLE_LIST light_sample_list
    )
{
    if (light_sampler == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (!PointValidate(point))
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (rng == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (light_sample_list == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    LightSampleCollectorClear(&light_sample_list->collector);

    ISTATUS status =
        light_sampler->vtable->sample_routine(light_sampler->data,
                                              point,
                                              rng,
                                              &light_sample_list->collector);

    return status;
}

ISTATUS
LightSamplerCacheColors(
    _In_ PCLIGHT_SAMPLER light_sampler,
    _In_ PCOLOR_CACHE color_cache
    )
{
    if (light_sampler == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (color_cache == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    ISTATUS status =
        light_sampler->vtable->cache_colors_routine(light_sampler->data,
                                                    color_cache);

    return status;
}

void
LightSamplerRetain(
    _In_opt_ PLIGHT_SAMPLER light_sampler
    )
{
    if (light_sampler == NULL)
    {
        return;
    }

    atomic_fetch_add(&light_sampler->reference_count, 1);
}

void
LightSamplerRelease(
    _In_opt_ _Post_invalid_ PLIGHT_SAMPLER light_sampler
    )
{
    if (light_sampler == NULL)
    {
        return;
    }

    if (atomic_fetch_sub(&light_sampler->reference_count, 1) == 1)
    {
        if (light_sampler->vtable->free_routine != NULL)
        {
            light_sampler->vtable->free_routine(light_sampler->data);
        }

        free(light_sampler);
    }
}