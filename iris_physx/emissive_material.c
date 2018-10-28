/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    emissive_material.c

Abstract:

    Definitions for the emissive material interface.

--*/

#include <stdalign.h>
#include <stdatomic.h>
#include <string.h>

#include "common/alloc.h"
#include "iris_physx/emissive_material.h"
#include "iris_physx/emissive_material_internal.h"

//
// Functions
//

ISTATUS
EmissiveMaterialAllocate(
    _In_ PCEMISSIVE_MATERIAL_VTABLE vtable,
    _In_reads_bytes_opt_(data_size) const void *data,
    _In_ size_t data_size,
    _In_ size_t data_alignment,
    _Out_ PEMISSIVE_MATERIAL *emissive_material
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

    if (emissive_material == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    void *data_allocation;
    bool success = AlignedAllocWithHeader(sizeof(EMISSIVE_MATERIAL),
                                          alignof(EMISSIVE_MATERIAL),
                                          (void **)emissive_material,
                                          data_size,
                                          data_alignment,
                                          &data_allocation);

    if (!success)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    (*emissive_material)->vtable = vtable;
    (*emissive_material)->data = data_allocation;
    (*emissive_material)->reference_count = 1;

    if (data_size != 0)
    {
        memcpy(data_allocation, data, data_size);
    }

    return ISTATUS_SUCCESS;
}

void
EmissiveMaterialRetain(
    _In_opt_ PEMISSIVE_MATERIAL emissive_material
    )
{
    if (emissive_material == NULL)
    {
        return;
    }

    atomic_fetch_add(&emissive_material->reference_count, 1);
}

void
EmissiveMaterialRelease(
    _In_opt_ _Post_invalid_ PEMISSIVE_MATERIAL emissive_material
    )
{
    if (emissive_material == NULL)
    {
        return;
    }

    if (atomic_fetch_sub(&emissive_material->reference_count, 1) == 1)
    {
        if (emissive_material->vtable->free_routine != NULL)
        {
            emissive_material->vtable->free_routine(emissive_material->data);
        }
    
        free(emissive_material);
    }
}