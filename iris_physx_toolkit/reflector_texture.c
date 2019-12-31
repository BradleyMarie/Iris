/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    reflector_texture.c

Abstract:

    Interface representing a texture of reflectors.

--*/

#include "common/alloc.h"
#include "iris_physx_toolkit/reflector_texture.h"

#include <stdalign.h>
#include <stdatomic.h>
#include <string.h>

//
// Types
//

struct _REFLECTOR_TEXTURE {
    PCREFLECTOR_TEXTURE_VTABLE vtable;
    void *data;
    atomic_uintmax_t reference_count;
};

//
// Functions
//

ISTATUS
ReflectorTextureAllocate(
    _In_ PCREFLECTOR_TEXTURE_VTABLE vtable,
    _In_reads_bytes_opt_(data_size) const void *data,
    _In_ size_t data_size,
    _In_ size_t data_alignment,
    _Out_ PREFLECTOR_TEXTURE *texture
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
    bool success = AlignedAllocWithHeader(sizeof(REFLECTOR_TEXTURE),
                                          alignof(REFLECTOR_TEXTURE),
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
ReflectorTextureSample(
    _In_opt_ PCREFLECTOR_TEXTURE texture,
    _In_ POINT3 model_hit_point,
    _In_ const void *additional_data,
    _In_ const void *texture_coordinates,
    _Inout_ PREFLECTOR_COMPOSITOR reflector_compositor,
    _Out_ PCREFLECTOR *value
    )
{
    if (!PointValidate(model_hit_point))
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (reflector_compositor == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    if (value == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }

    if (texture == NULL)
    {
        *value = NULL;
        return ISTATUS_SUCCESS;
    }

    ISTATUS status = texture->vtable->sample_routine(texture->data,
                                                     model_hit_point,
                                                     additional_data,
                                                     texture_coordinates,
                                                     reflector_compositor,
                                                     value);

    return status;
}

void
ReflectorTextureRetain(
    _In_opt_ PREFLECTOR_TEXTURE texture
    )
{
    if (texture == NULL)
    {
        return;
    }

    atomic_fetch_add(&texture->reference_count, 1);
}

void
ReflectorTextureRelease(
    _In_opt_ _Post_invalid_ PREFLECTOR_TEXTURE texture
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