/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    random.c

Abstract:

    Interface for random number generation.

--*/

#include <stdalign.h>
#include <string.h>

#include "iris_camera/pixel_sampler_generator.h"
#include "iris_camera/pixel_sampler_generator_internal.h"

//
// Functions
//

ISTATUS
PixelSamplerGeneratorAllocate(
    _In_ PCPIXEL_SAMPLER_GENERATOR_VTABLE vtable,
    _In_reads_bytes_opt_(data_size) const void *data,
    _In_ size_t data_size,
    _In_ size_t data_alignment,
    _Out_ PPIXEL_SAMPLER_GENERATOR *pixel_sampler_generator
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

    if (pixel_sampler_generator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    void *data_allocation;
    bool success = AlignedAllocWithHeader(sizeof(PIXEL_SAMPLER_GENERATOR),
                                          alignof(PIXEL_SAMPLER_GENERATOR),
                                          (void **)pixel_sampler_generator,
                                          data_size,
                                          data_alignment,
                                          &data_allocation);

    if (!success)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    (*pixel_sampler_generator)->vtable = vtable;
    (*pixel_sampler_generator)->data = data_allocation;

    if (data_size != 0)
    {
        memcpy(data_allocation, data, data_size);
    }

    return ISTATUS_SUCCESS;
}

void
PixelSamplerGeneratorFree(
    _In_opt_ _Post_invalid_ PPIXEL_SAMPLER_GENERATOR pixel_sampler_generator
    )
{
    if (pixel_sampler_generator == NULL)
    {
        return;
    }

    if (pixel_sampler_generator->vtable->free_routine)
    {
        pixel_sampler_generator->vtable->free_routine(
            pixel_sampler_generator->data);
    }

    free(pixel_sampler_generator);
}