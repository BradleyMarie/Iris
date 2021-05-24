/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    texture_coordinate_map.c

Abstract:

    Interface representing a texture coordinate map.

--*/

#include <stdalign.h>
#include <string.h>

#include "common/alloc.h"
#include "iris_physx/texture_coordinate_map.h"
#include "iris_physx/texture_coordinate_map_internal.h"

//
// Functions
//

ISTATUS
TextureCoordinateMapAllocate(
    _In_ PCTEXTURE_COORDINATE_MAP_VTABLE vtable,
    _In_reads_bytes_opt_(data_size) const void *data,
    _In_ size_t data_size,
    _In_ size_t data_alignment,
    _Out_ PTEXTURE_COORDINATE_MAP *texture_coordinate_map
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

    if (texture_coordinate_map == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    void *data_allocation;
    bool success = AlignedAllocWithHeader(sizeof(TEXTURE_COORDINATE_MAP),
                                          alignof(TEXTURE_COORDINATE_MAP),
                                          (void **)texture_coordinate_map,
                                          data_size,
                                          data_alignment,
                                          &data_allocation);

    if (!success)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    (*texture_coordinate_map)->vtable = vtable;
    (*texture_coordinate_map)->data = data_allocation;
    (*texture_coordinate_map)->reference_count = 1;

    if (data_size != 0)
    {
        memcpy(data_allocation, data, data_size);
    }

    return ISTATUS_SUCCESS;
}

void
TextureCoordinateMapRetain(
    _In_opt_ PTEXTURE_COORDINATE_MAP texture_coordinate_map
    )
{
    if (texture_coordinate_map == NULL)
    {
        return;
    }

    atomic_fetch_add(&texture_coordinate_map->reference_count, 1);
}

void
TextureCoordinateMapRelease(
    _In_opt_ _Post_invalid_ PTEXTURE_COORDINATE_MAP texture_coordinate_map
    )
{
    if (texture_coordinate_map == NULL)
    {
        return;
    }

    if (atomic_fetch_sub(&texture_coordinate_map->reference_count, 1) == 1)
    {
        if (texture_coordinate_map->vtable->free_routine != NULL)
        {
            texture_coordinate_map->vtable->free_routine(texture_coordinate_map->data);
        }
    
        free(texture_coordinate_map);
    }
}