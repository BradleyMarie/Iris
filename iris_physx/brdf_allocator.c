/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    brdf_allocator.c

Abstract:

    Allows allocation of BRDFs cheaply during shading.

--*/

#include "iris_physx/brdf_allocator.h"

#include <assert.h>
#include <string.h>

#include "iris_physx/brdf_allocator_internal.h"

//
// Functions
//

ISTATUS
BrdfAllocatorAllocate(
    _Inout_ PBRDF_ALLOCATOR allocator,
    _In_ PCBRDF_VTABLE vtable,
    _In_reads_bytes_opt_(data_size) const void *data,
    _In_ size_t data_size,
    _In_ size_t data_alignment,
    _Out_ PCBRDF *brdf
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

    if (brdf == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }

    PBRDF brdf_allocation;
    void *data_allocation;
    bool success = DynamicMemoryAllocatorAllocate(&allocator->allocator,
                                                  sizeof(BRDF),
                                                  alignof(BRDF),
                                                  (void **)&brdf_allocation,
                                                  data_size,
                                                  data_alignment,
                                                  &data_allocation);

    if (!success)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    BrdfInitialize(vtable,
                   data_allocation,
                   brdf_allocation);

    if (data_size != 0)
    {
        memcpy(data_allocation, data, data_size);
    }

    *brdf = brdf_allocation;

    return ISTATUS_SUCCESS;
}