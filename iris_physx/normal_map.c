/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    normal_map.c

Abstract:

    Interface representing a normal map.

--*/

#include <stdalign.h>
#include <string.h>

#include "common/alloc.h"
#include "iris_physx/normal_map.h"
#include "iris_physx/normal_map_internal.h"

//
// Functions
//

ISTATUS
NormalMapAllocate(
    _In_ PCNORMAL_MAP_VTABLE vtable,
    _In_reads_bytes_opt_(data_size) const void *data,
    _In_ size_t data_size,
    _In_ size_t data_alignment,
    _Out_ PNORMAL_MAP *normal_map
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

    if (normal_map == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    void *data_allocation;
    bool success = AlignedAllocWithHeader(sizeof(NORMAL_MAP),
                                          alignof(NORMAL_MAP),
                                          (void **)normal_map,
                                          data_size,
                                          data_alignment,
                                          &data_allocation);

    if (!success)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    (*normal_map)->vtable = vtable;
    (*normal_map)->data = data_allocation;
    (*normal_map)->reference_count = 1;

    if (data_size != 0)
    {
        memcpy(data_allocation, data, data_size);
    }

    return ISTATUS_SUCCESS;
}

void
NormalMapRetain(
    _In_opt_ PNORMAL_MAP normal_map
    )
{
    if (normal_map == NULL)
    {
        return;
    }

    atomic_fetch_add(&normal_map->reference_count, 1);
}

void
NormalMapRelease(
    _In_opt_ _Post_invalid_ PNORMAL_MAP normal_map
    )
{
    if (normal_map == NULL)
    {
        return;
    }

    if (atomic_fetch_sub(&normal_map->reference_count, 1) == 1)
    {
        if (normal_map->vtable->free_routine != NULL)
        {
            normal_map->vtable->free_routine(normal_map->data);
        }
    
        free(normal_map);
    }
}