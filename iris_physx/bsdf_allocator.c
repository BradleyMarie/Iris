/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    bsdf_allocator.c

Abstract:

    Allows allocation of BSDFs cheaply during shading.

--*/

#include "iris_physx/bsdf_allocator.h"

#include <assert.h>
#include <string.h>

#include "iris_physx/bsdf_allocator_internal.h"

//
// Functions
//

ISTATUS
BsdfAllocatorAllocate(
    _Inout_ PBSDF_ALLOCATOR allocator,
    _In_ PCBSDF_VTABLE vtable,
    _In_reads_bytes_opt_(data_size) const void *data,
    _In_ size_t data_size,
    _In_ size_t data_alignment,
    _Out_ PCBSDF *bsdf
    )
{
    if (allocator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (vtable == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
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

    if (bsdf == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }

    PBSDF bsdf_allocation;
    void *data_allocation;
    bool success = DynamicMemoryAllocatorAllocate(&allocator->allocator,
                                                  sizeof(BSDF),
                                                  alignof(BSDF),
                                                  (void **)&bsdf_allocation,
                                                  data_size,
                                                  data_alignment,
                                                  &data_allocation);

    if (!success)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    BsdfInitialize(vtable,
                   data_allocation,
                   bsdf_allocation);

    if (data_size != 0)
    {
        memcpy(data_allocation, data, data_size);
    }

    *bsdf = bsdf_allocation;

    return ISTATUS_SUCCESS;
}