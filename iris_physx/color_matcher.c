/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    color_matcher.c

Abstract:

    Interface representing a color matcher.

--*/

#include <stdalign.h>
#include <string.h>

#include "common/alloc.h"
#include "iris_physx/color_matcher.h"
#include "iris_physx/color_matcher_internal.h"

//
// Functions
//

ISTATUS
ColorMatcherAllocate(
    _In_ PCCOLOR_MATCHER_VTABLE vtable,
    _In_reads_bytes_opt_(data_size) const void *data,
    _In_ size_t data_size,
    _In_ size_t data_alignment,
    _Out_ PCOLOR_MATCHER *color_matcher
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

    if (color_matcher == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    void *data_allocation;
    bool success = AlignedAllocWithHeader(sizeof(COLOR_MATCHER),
                                          alignof(COLOR_MATCHER),
                                          (void **)color_matcher,
                                          data_size,
                                          data_alignment,
                                          &data_allocation);

    if (!success)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    ColorMatcherInitialize(vtable,
                           data_allocation,
                           *color_matcher);

    if (data_size != 0)
    {
        memcpy(data_allocation, data, data_size);
    }

    return ISTATUS_SUCCESS;
}

void
ColorMatcherRetain(
    _In_opt_ PCOLOR_MATCHER color_matcher
    )
{
    if (color_matcher == NULL)
    {
        return;
    }

    atomic_fetch_add(&color_matcher->reference_count, 1);
}

void
ColorMatcherRelease(
    _In_opt_ _Post_invalid_ PCOLOR_MATCHER color_matcher
    )
{
    if (color_matcher == NULL)
    {
        return;
    }

    if (atomic_fetch_sub(&color_matcher->reference_count, 1) == 1)
    {
        if (color_matcher->vtable->free_routine != NULL)
        {
            color_matcher->vtable->free_routine(color_matcher->data);
        }

        free(color_matcher);
    }
}