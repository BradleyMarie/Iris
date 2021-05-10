/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    spectrum_texture.c

Abstract:

    Interface representing a texture of spectrums.

--*/

#include "common/alloc.h"
#include "iris_physx_toolkit/spectrum_texture.h"

#include <stdalign.h>
#include <stdatomic.h>
#include <string.h>

//
// Types
//

struct _SPECTRUM_TEXTURE {
    PCSPECTRUM_TEXTURE_VTABLE vtable;
    void *data;
    atomic_uintmax_t reference_count;
};

//
// Functions
//

ISTATUS
SpectrumTextureAllocate(
    _In_ PCSPECTRUM_TEXTURE_VTABLE vtable,
    _In_reads_bytes_opt_(data_size) const void *data,
    _In_ size_t data_size,
    _In_ size_t data_alignment,
    _Out_ PSPECTRUM_TEXTURE *texture
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
    bool success = AlignedAllocWithHeader(sizeof(SPECTRUM_TEXTURE),
                                          alignof(SPECTRUM_TEXTURE),
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
SpectrumTextureSample(
    _In_opt_ PCSPECTRUM_TEXTURE texture,
    _In_ VECTOR3 direction,
    _In_opt_ VECTOR3 dxdy[2],
    _Inout_ PSPECTRUM_COMPOSITOR spectrum_compositor,
    _Out_ PCSPECTRUM *value
    )
{
    if (!VectorValidate(direction))
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (dxdy != NULL &&
        (!VectorValidate(dxdy[0]) || !VectorValidate(dxdy[1])))
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (spectrum_compositor == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (value == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    if (texture == NULL)
    {
        *value = NULL;
        return ISTATUS_SUCCESS;
    }

    ISTATUS status = texture->vtable->sample_routine(texture->data,
                                                     direction,
                                                     dxdy,
                                                     spectrum_compositor,
                                                     value);

    return status;
}

void
SpectrumTextureRetain(
    _In_opt_ PSPECTRUM_TEXTURE texture
    )
{
    if (texture == NULL)
    {
        return;
    }

    atomic_fetch_add(&texture->reference_count, 1);
}

void
SpectrumTextureRelease(
    _In_opt_ _Post_invalid_ PSPECTRUM_TEXTURE texture
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