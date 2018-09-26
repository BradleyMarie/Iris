/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    random_generator.c

Abstract:

    Interface for random generator.

--*/

#include <stdalign.h>
#include <string.h>

#include "iris_camera/random_generator.h"
#include "iris_camera/random_generator_internal.h"

//
// Functions
//

ISTATUS
RandomGeneratorAllocate(
    _In_ PCRANDOM_GENERATOR_VTABLE vtable,
    _In_reads_bytes_opt_(data_size) const void *data,
    _In_ size_t data_size,
    _In_ size_t data_alignment,
    _Out_ PRANDOM_GENERATOR *random_generator
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

    if (random_generator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    void *data_allocation;
    bool success = AlignedAllocWithHeader(sizeof(RANDOM_GENERATOR),
                                          alignof(RANDOM_GENERATOR),
                                          (void **)random_generator,
                                          data_size,
                                          data_alignment,
                                          &data_allocation);

    if (!success)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    (*random_generator)->vtable = vtable;
    (*random_generator)->data = data_allocation;

    if (data_size != 0)
    {
        memcpy(data_allocation, data, data_size);
    }

    return ISTATUS_SUCCESS;
}

void
RandomGeneratorFree(
    _In_opt_ _Post_invalid_ PRANDOM_GENERATOR random_generator
    )
{
    if (random_generator == NULL)
    {
        return;
    }

    if (random_generator->vtable->free_routine)
    {
        random_generator->vtable->free_routine(random_generator->data);
    }

    free(random_generator);
}