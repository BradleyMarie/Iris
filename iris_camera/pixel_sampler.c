/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    pixel_sampler.c

Abstract:

    Samples the UV space on the lens and framebuffer.

--*/

#include <stdalign.h>
#include <string.h>

#include "common/alloc.h"
#include "iris_camera/pixel_sampler_internal.h"
#include "iris_camera/pixel_sampler.h"

//
// Functions
//

ISTATUS
PixelSamplerAllocate(
    _In_ PCPIXEL_SAMPLER_VTABLE vtable,
    _In_reads_bytes_opt_(data_size) const void *data,
    _In_ size_t data_size,
    _In_ size_t data_alignment,
    _Out_ PPIXEL_SAMPLER *pixel_sampler
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

    if (pixel_sampler == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    void *data_allocation;
    bool success = AlignedAllocWithHeader(sizeof(PIXEL_SAMPLER),
                                          alignof(PIXEL_SAMPLER),
                                          (void **)pixel_sampler,
                                          data_size,
                                          data_alignment,
                                          &data_allocation);

    if (!success)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    (*pixel_sampler)->vtable = vtable;
    (*pixel_sampler)->data = data_allocation;

    if (data_size != 0)
    {
        memcpy(data_allocation, data, data_size);
    }

    return ISTATUS_SUCCESS;
}

void
PixelSamplerFree(
    _In_opt_ _Post_invalid_ PPIXEL_SAMPLER pixel_sampler
    )
{
    if (pixel_sampler == NULL)
    {
        return;
    }

    if (pixel_sampler->vtable->free_routine != NULL)
    {
        pixel_sampler->vtable->free_routine(pixel_sampler->data);
    }

    free(pixel_sampler);
}