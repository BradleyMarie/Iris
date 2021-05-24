/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    material.c

Abstract:

    Definitions for the material interface.

--*/

#include <stdalign.h>
#include <stdatomic.h>
#include <string.h>

#include "common/alloc.h"
#include "iris_physx/material.h"
#include "iris_physx/material_internal.h"

//
// Functions
//

ISTATUS
MaterialAllocate(
    _In_ PCMATERIAL_VTABLE vtable,
    _In_reads_bytes_opt_(data_size) const void *data,
    _In_ size_t data_size,
    _In_ size_t data_alignment,
    _Out_ PMATERIAL *material
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

    if (material == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    void *data_allocation;
    bool success = AlignedAllocWithHeader(sizeof(MATERIAL),
                                          alignof(MATERIAL),
                                          (void **)material,
                                          data_size,
                                          data_alignment,
                                          &data_allocation);

    if (!success)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    (*material)->vtable = vtable;
    (*material)->data = data_allocation;
    (*material)->reference_count = 1;

    if (data_size != 0)
    {
        memcpy(data_allocation, data, data_size);
    }

    return ISTATUS_SUCCESS;
}

ISTATUS
MaterialSample(
    _In_ PCMATERIAL material,
    _In_ PCINTERSECTION intersection,
    _In_ const void *additional_data,
    _In_ const void *texture_coordinates,
    _Inout_ PBSDF_ALLOCATOR bsdf_allocator,
    _Inout_ PREFLECTOR_COMPOSITOR reflector_compositor,
    _Out_ PCBSDF *bsdf
    )
{
    if (material == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (intersection == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (bsdf_allocator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    if (reflector_compositor == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }

    if (bsdf == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_06;
    }

    ISTATUS status = MaterialSampleInternal(material,
                                            intersection,
                                            additional_data,
                                            texture_coordinates,
                                            bsdf_allocator,
                                            reflector_compositor,
                                            bsdf);

    return status;
}

void
MaterialRetain(
    _In_opt_ PMATERIAL material
    )
{
    if (material == NULL)
    {
        return;
    }

    atomic_fetch_add(&material->reference_count, 1);
}

void
MaterialRelease(
    _In_opt_ _Post_invalid_ PMATERIAL material
    )
{
    if (material == NULL)
    {
        return;
    }

    if (atomic_fetch_sub(&material->reference_count, 1) == 1)
    {
        if (material->vtable->free_routine != NULL)
        {
            material->vtable->free_routine(material->data);
        }
    
        free(material);
    }
}