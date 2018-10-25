/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    light_sampler.c

Abstract:

    Implements a light sampler.

--*/

#include <stdalign.h>
#include <string.h>

#include "common/alloc.h"
#include "iris_physx/light_sampler.h"

//
// Types
//

struct _LIGHT_SAMPLER {
    PCLIGHT_SAMPLER_VTABLE vtable;
    bool prepared;
    void *data;
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
    (*light_sampler)->prepared = false;
    (*light_sampler)->data = data_allocation;

    if (data_size != 0)
    {
        memcpy(data_allocation, data, data_size);
    }

    return ISTATUS_SUCCESS;
}

ISTATUS
LightSamplerPrepareSamples(
    _Inout_ PLIGHT_SAMPLER light_sampler,
    _In_ POINT3 point
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

    ISTATUS status = 
        light_sampler->vtable->prepare_samples_routine(light_sampler->data,
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
    _Out_ PCLIGHT *light,
    _Out_ float_t *pdf
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

    if (pdf == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (!light_sampler->prepared)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    ISTATUS status = 
        light_sampler->vtable->next_sample_routine(light_sampler->data,
                                                   light,
                                                   pdf);

    if (status == ISTATUS_DONE)
    {
        light_sampler->prepared = false;
    }

    return status;
}

void
LightSamplerFree(
    _In_opt_ _Post_invalid_ PLIGHT_SAMPLER light_sampler
    )
{
    if (light_sampler == NULL)
    {
        return;
    }

    if (light_sampler->vtable->free_routine != NULL)
    {
        light_sampler->vtable->free_routine(light_sampler->data);
    }

    free(light_sampler);
}