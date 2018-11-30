/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    tone_mapper.c

Abstract:

    Interface representing a tone mapper.

--*/

#include <stdalign.h>
#include <string.h>

#include "common/alloc.h"
#include "iris_physx/tone_mapper.h"
#include "iris_physx/tone_mapper_internal.h"

//
// Functions
//

ISTATUS
ToneMapperAllocate(
    _In_ PCTONE_MAPPER_VTABLE vtable,
    _In_reads_bytes_opt_(data_size) const void *data,
    _In_ size_t data_size,
    _In_ size_t data_alignment,
    _Out_ PTONE_MAPPER *tone_mapper
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

    if (tone_mapper == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    void *data_allocation;
    bool success = AlignedAllocWithHeader(sizeof(TONE_MAPPER),
                                          alignof(TONE_MAPPER),
                                          (void **)tone_mapper,
                                          data_size,
                                          data_alignment,
                                          &data_allocation);

    if (!success)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    ToneMapperInitialize(vtable,
                         data_allocation,
                         *tone_mapper);

    if (data_size != 0)
    {
        memcpy(data_allocation, data, data_size);
    }

    return ISTATUS_SUCCESS;
}

ISTATUS
ToneMapperComputeTone(
    _In_ PCTONE_MAPPER tone_mapper,
    _Out_ PCOLOR3 color
    )
{
    if (tone_mapper == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (color == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    ISTATUS status =
        tone_mapper->vtable->compute_tone_routine(tone_mapper->data,
                                                  color);

    return status;
}

ISTATUS
ToneMapperClear(
    _In_ PTONE_MAPPER tone_mapper
    )
{
    if (tone_mapper == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    ISTATUS status = tone_mapper->vtable->clear_routine(tone_mapper->data);

    return status;
}

void
ToneMapperFree(
    _In_opt_ _Post_invalid_ PTONE_MAPPER tone_mapper
    )
{
    if (tone_mapper == NULL)
    {
        return;
    }

    if (tone_mapper->vtable->free_routine != NULL)
    {
        tone_mapper->vtable->free_routine(tone_mapper->data);
    }

    free(tone_mapper);
}