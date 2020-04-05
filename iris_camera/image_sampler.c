/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    image_sampler.c

Abstract:

    Samples the UV space on the lens and framebuffer.

--*/

#include <stdalign.h>
#include <string.h>

#include "common/alloc.h"
#include "iris_camera/image_sampler_internal.h"
#include "iris_camera/image_sampler.h"

//
// Functions
//

ISTATUS
ImageSamplerAllocate(
    _In_ PCIMAGE_SAMPLER_VTABLE vtable,
    _In_reads_bytes_opt_(data_size) const void *data,
    _In_ size_t data_size,
    _In_ size_t data_alignment,
    _Out_ PIMAGE_SAMPLER *image_sampler
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

    if (image_sampler == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    void *data_allocation;
    bool success = AlignedAllocWithHeader(sizeof(IMAGE_SAMPLER),
                                          alignof(IMAGE_SAMPLER),
                                          (void **)image_sampler,
                                          data_size,
                                          data_alignment,
                                          &data_allocation);

    if (!success)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    (*image_sampler)->vtable = vtable;
    (*image_sampler)->data = data_allocation;

    if (data_size != 0)
    {
        memcpy(data_allocation, data, data_size);
    }

    return ISTATUS_SUCCESS;
}

void
ImageSamplerFree(
    _In_opt_ _Post_invalid_ PIMAGE_SAMPLER image_sampler
    )
{
    if (image_sampler == NULL)
    {
        return;
    }

    if (image_sampler->vtable->free_routine != NULL)
    {
        image_sampler->vtable->free_routine(image_sampler->data);
    }

    free(image_sampler);
}