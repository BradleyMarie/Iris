/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    reflector_allocator.c

Abstract:

    Allows allocation of reflectors cheaply during shading.

--*/

#include "iris_physx/reflector_allocator.h"

#include <assert.h>
#include <string.h>

#include "iris_physx/reflector_allocator_internal.h"

//
// Functions
//

ISTATUS
ReflectorAllocatorAllocate(
    _Inout_ PREFLECTOR_ALLOCATOR allocator,
    _In_ PCREFLECTOR_VTABLE vtable,
    _In_reads_bytes_opt_(data_size) const void *data,
    _In_ size_t data_size,
    _In_ size_t data_alignment,
    _Out_ PCREFLECTOR *reflector
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

    if (reflector == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }

    PREFLECTOR reflector_allocation;
    void *data_allocation;
    bool success = DynamicMemoryAllocatorAllocate(&allocator->allocator,
                                                  sizeof(REFLECTOR),
                                                  alignof(REFLECTOR),
                                                  (void **)&reflector_allocation,
                                                  data_size,
                                                  data_alignment,
                                                  &data_allocation);

    if (!success)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    ReflectorInitialize(vtable,
                        data_allocation,
                        reflector_allocation);

    if (data_size != 0)
    {
        memcpy(data_allocation, data, data_size);
    }

    *reflector = reflector_allocation;

    return ISTATUS_SUCCESS;
}