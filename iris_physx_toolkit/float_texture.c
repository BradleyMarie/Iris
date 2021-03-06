/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    float_texture.c

Abstract:

    Interface representing a texture of floats.

--*/

#include "common/alloc.h"
#include "iris_physx_toolkit/float_texture.h"

#include <stdalign.h>
#include <stdatomic.h>
#include <string.h>

//
// Types
//

struct _FLOAT_TEXTURE {
    PCFLOAT_TEXTURE_VTABLE vtable;
    void *data;
    atomic_uintmax_t reference_count;
};

//
// Functions
//

ISTATUS
FloatTextureAllocate(
    _In_ PCFLOAT_TEXTURE_VTABLE vtable,
    _In_reads_bytes_opt_(data_size) const void *data,
    _In_ size_t data_size,
    _In_ size_t data_alignment,
    _Out_ PFLOAT_TEXTURE *texture
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

    if (texture == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    void *data_allocation;
    bool success = AlignedAllocWithHeader(sizeof(FLOAT_TEXTURE),
                                          alignof(FLOAT_TEXTURE),
                                          (void **)texture,
                                          data_size,
                                          data_alignment,
                                          &data_allocation);

    if (!success)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    (*texture)->vtable = vtable;
    (*texture)->data = data_allocation;
    (*texture)->reference_count = 1;

    if (data_size != 0)
    {
        memcpy(data_allocation, data, data_size);
    }

    return ISTATUS_SUCCESS;
}

ISTATUS
FloatTextureSample(
    _In_opt_ PCFLOAT_TEXTURE texture,
    _In_ PCINTERSECTION intersection,
    _In_opt_ const void *additional_data,
    _In_opt_ const void *texture_coordinates,
    _Out_ float_t *value
    )
{
    if (intersection == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (value == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    if (texture == NULL)
    {
        *value = (float_t)0.0;
        return ISTATUS_SUCCESS;
    }

    ISTATUS status = texture->vtable->sample_routine(texture->data,
                                                     intersection,
                                                     additional_data,
                                                     texture_coordinates,
                                                     value);

    return status;
}

void
FloatTextureRetain(
    _In_opt_ PFLOAT_TEXTURE texture
    )
{
    if (texture == NULL)
    {
        return;
    }

    atomic_fetch_add(&texture->reference_count, 1);
}

void
FloatTextureRelease(
    _In_opt_ _Post_invalid_ PFLOAT_TEXTURE texture
    )
{
    if (texture == NULL)
    {
        return;
    }

    if (atomic_fetch_sub(&texture->reference_count, 1) == 1)
    {
        if (texture->vtable->free_routine != NULL)
        {
            texture->vtable->free_routine(texture->data);
        }
    
        free(texture);
    }
}