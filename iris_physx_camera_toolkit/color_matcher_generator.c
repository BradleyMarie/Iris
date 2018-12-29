/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    color_matcher_generator.c

Abstract:

    Interface for color matcher generators.

--*/

#include <stdalign.h>
#include <string.h>

#include "common/alloc.h"
#include "iris_physx_camera_toolkit/color_matcher_generator.h"
#include "iris_physx_camera_toolkit/color_matcher_generator_internal.h"

//
// Functions
//

ISTATUS
ColorMatcherGeneratorAllocate(
    _In_ PCCOLOR_MATCHER_GENERATOR_VTABLE vtable,
    _In_reads_bytes_opt_(data_size) const void *data,
    _In_ size_t data_size,
    _In_ size_t data_alignment,
    _Out_ PCOLOR_MATCHER_GENERATOR *color_matcher_generator
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

    if (color_matcher_generator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    void *data_allocation;
    bool success = AlignedAllocWithHeader(sizeof(COLOR_MATCHER_GENERATOR),
                                          alignof(COLOR_MATCHER_GENERATOR),
                                          (void **)color_matcher_generator,
                                          data_size,
                                          data_alignment,
                                          &data_allocation);

    if (!success)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    (*color_matcher_generator)->vtable = vtable;
    (*color_matcher_generator)->data = data_allocation;

    if (data_size != 0)
    {
        memcpy(data_allocation, data, data_size);
    }

    return ISTATUS_SUCCESS;
}

void
ColorMatcherGeneratorFree(
    _In_opt_ _Post_invalid_ PCOLOR_MATCHER_GENERATOR color_matcher_generator
    )
{
    if (color_matcher_generator == NULL)
    {
        return;
    }

    if (color_matcher_generator->vtable->free_routine)
    {
        color_matcher_generator->vtable->free_routine(
            color_matcher_generator->data);
    }

    free(color_matcher_generator);
}